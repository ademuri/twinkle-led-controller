#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <esp_wifi.h>
#include <iomanip>
#include <rom/rtc.h>
#include <string>
#include <vector>

#include "constants.h"
#include "html.h"

enum class Mode {
  kBright,
  kTwinkle,
  kHalf,
  kOff,
};

struct Strand {
  uint8_t brightness_a;
  uint8_t brightness_b;

  const int pin_0;
  const int pin_1;
};

static std::vector<Strand> strands = {
  {0, 0, 13, 15}, //1
  {0, 0, 5, 18},  //2
  {0, 0, 22, 23}, //3
  {0, 0, 27, 14}, //4
  {0, 0, 17, 16}, //5
  {0, 0, 4, 2},   //6
  {0, 0, 19, 21}, //7
  {0, 0, 33, 32}, //8
  {0, 0, 26, 25}, //9
};
static const uint8_t kNumPwmStrands = 8;

WiFiClient wifiClient;
PubSubClient pubSub(wifiClient);
AsyncWebServer server(80);

Mode mode = Mode::kOff;
uint8_t max_brightness = 255;

static const char* kMdnsName = "twinkle-controller";
static const uint32_t kRefreshMdnsDelay = 60 * 1000;

static const char* kMqttUser = "mqtt";
static const char* kMqttPassword = "mypassword";
static const char* kCommandTopic = "home/twinkle/command";

// Timer stuff, used for strand control. See the ESP32 timer example:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Timer/RepeatTimer/RepeatTimer.ino
// Used to disable task switching during ISR
portMUX_TYPE pwmTimerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE bitBangTimerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * pwmTimer = nullptr;

// This is ~the fastest frequency at which the PWM/LEDC will mostly match the
// software PWM.
static const double kPwmFreq = 78125.0 / 4.0;
static const uint8_t kPwmResolution = 8;

static const int kLedPin = 12;

void IRAM_ATTR updatePwmLeds() {
  static uint8_t alternator = 0;
  static uint8_t strand_index = 0;
  static uint8_t pwm_counter = 0;

  portENTER_CRITICAL_ISR(&pwmTimerMux);
  updateBitBangLeds();

  pwm_counter++;
  if (pwm_counter > 8) {
    pwm_counter = 0;
    Strand strand = strands[strand_index];
    if (alternator == 0) {
      ledcWrite(strand_index, 0);
      ledcWrite(kNumPwmStrands + strand_index, strand.brightness_b);
    } else {
      ledcWrite(kNumPwmStrands + strand_index, 0);
      ledcWrite(strand_index, strand.brightness_a);
    }
    strand_index--;
    if (strand_index > kNumPwmStrands) {
      strand_index = kNumPwmStrands - 1;
      if (alternator == 0) {
        alternator = 1;
      } else {
        alternator = 0;
      }
    }
  }

  portEXIT_CRITICAL_ISR(&pwmTimerMux);
}

void IRAM_ATTR updateBitBangLeds() {
  static uint16_t pwm_counter = 0;
  static uint8_t alternator = 0;

  for (uint32_t i = kNumPwmStrands; i < strands.size(); i++) {
    Strand strand = strands[i];
    if (alternator == 0) {
      digitalWrite(strand.pin_0, LOW);
      digitalWrite(strand.pin_1, pwm_counter < strands[i].brightness_b);
    } else {
      digitalWrite(strand.pin_1, LOW);
      digitalWrite(strand.pin_0, pwm_counter < strands[i].brightness_a);
    }
  }
  if (alternator == 0) {
    alternator = 1;
  } else {
    alternator = 0;
    pwm_counter++;
    if (pwm_counter > 255) {
      pwm_counter = 0;
    }
  }
}

String htmlTemplateProcessor(const String &var) {
  if (var == "RESET") {
    return String(rtc_get_reset_reason(0)) + String(" ") + String(rtc_get_reset_reason(1));
  }
  return String();
}

void lightsTwinkle() {
  mode = Mode::kTwinkle;
  for (int j = 0; j < strands.size(); j++) {
    strands[j].brightness_a = 0;
    strands[j].brightness_b = 0;
  }
}

void handleMqtt(const char* topic, byte* payload, unsigned int length) {
  Serial.println("Received MQTT");
  // Assume that this is the command topic, since that's the only one we subscribe to.
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.printf("Failed to parse JSON: %s", error.c_str());
    return;
  }

  if (doc.containsKey("state")) {
    if (strcmp(doc["state"], "ON") == 0) {
      mode = Mode::kBright;
    } else if (strcmp(doc["state"], "OFF") == 0) {
      mode = Mode::kOff;
    }
  }

  if (doc.containsKey("effect")) {
    if (strcmp(doc["effect"], "twinkle") == 0) {
      lightsTwinkle();
    } else if (strcmp(doc["effect"], "half") == 0) {
      mode = Mode::kHalf;
    } else if (strcmp(doc["effect"], "normal") == 0) {
      mode = Mode::kBright;
    }
  }

  if (doc.containsKey("brightness")) {
    max_brightness = doc["brightness"];
  }
}

void setup() {
  // Note: setting the frequency in platformio.ini does nothing.
  setCpuFrequencyMhz(240);

  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, HIGH);
  Serial.begin(115200);
  Serial.println("Booted.");
  Serial.println("");

  for (uint32_t i = 0; i < kNumPwmStrands; i++) {
    Strand strand = strands[i];
    pinMode(strand.pin_0, OUTPUT);
    pinMode(strand.pin_1, OUTPUT);
    ledcSetup(i, kPwmFreq, kPwmResolution);
    ledcAttachPin(strand.pin_0, i);
    ledcSetup(kNumPwmStrands + i, kPwmFreq, kPwmResolution);
    ledcAttachPin(strand.pin_1, kNumPwmStrands + i);
  }
  for (uint32_t i = kNumPwmStrands; i < strands.size(); i++) {
    Strand strand = strands[i];
    pinMode(strand.pin_0, OUTPUT);
    digitalWrite(strand.pin_0, LOW);
    pinMode(strand.pin_1, OUTPUT);
    digitalWrite(strand.pin_1, LOW);
  }

  Serial.print("Connecting to wifi: ");
  WiFi.setSleep(false);
  esp_wifi_set_ps(WIFI_PS_NONE);
  WiFi.begin(kSsid, kPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Done.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  ArduinoOTA
    .onStart([]() {
      // This interrupt interferes with the update process
      timerAlarmDisable(pwmTimer);

      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_SPIFFS
        type = "filesystem";
      }

      Serial.println("Start updating " + type);
    });
  ArduinoOTA
    .onEnd([]() {
      Serial.println("\nEnd");
    });
  ArduinoOTA
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
  ArduinoOTA
    .onError([](ota_error_t error) {
      timerAlarmEnable(pwmTimer);

      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.setTimeout(5 * 1000);
  ArduinoOTA.begin();

  if (MDNS.begin(kMdnsName)) {
    // Add service to MDNS-SD
    MDNS.addService("_http", "_tcp", 80);
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, htmlTemplateProcessor);
  });
  // Note: use POST, not PUT, so that we don't have to include the regex
  // library for URL matching, which is large
  server.on("/bright", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kBright;
  });
  server.on("/twinkle", HTTP_POST, [](AsyncWebServerRequest *request) {
    lightsTwinkle();
  });
  server.on("/off", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kOff;
  });

  mode = Mode::kHalf;

  server.begin();
  Serial.println("Started server.");
  digitalWrite(kLedPin, LOW);

  pwmTimer = timerBegin(0, 240, true);
  timerAttachInterrupt(pwmTimer, &updatePwmLeds, true);
  // Call at 100kHz
  timerAlarmWrite(pwmTimer, 10, /* repeat */ true);
  timerAlarmEnable(pwmTimer);

  // TODO: get MQTT server via MDNS rather than a hard-coded IP
  //pubSub.setServer(MDNS.queryHost("home-assistant"), 1883);
  pubSub.setServer("192.168.86.33", 1883);
  pubSub.setCallback(handleMqtt);
  connect();
}

void connect() {
  while (!pubSub.connected()) {
    Serial.println("Connecting to MQTT");
    if (!pubSub.connect("twinkle", kMqttUser, kMqttPassword)) {
      Serial.println("Failed to init MQTT");
      delay(5000);
      continue;
    }
    pubSub.subscribe(kCommandTopic);
  }
}

void loop() {
  ArduinoOTA.handle();

  connect();
  pubSub.loop();

  switch(mode) {
    case Mode::kBright:
      for (int i = 0; i < strands.size(); i++) {
          strands[i].brightness_a = max_brightness;
          strands[i].brightness_b = max_brightness;
      }
      break;
    case Mode::kTwinkle:
      twinkle();
      break;
    case Mode::kHalf:
        for (int j = 0; j < strands.size(); j++) {
          strands[j].brightness_a = 0;
          strands[j].brightness_b = max_brightness;
        }
      break;
    case Mode::kOff:
      for (int j = 0; j < strands.size(); j++) {
        strands[j].brightness_a = 0;
        strands[j].brightness_b = 0;
      }
      break;
    }
}

void twinkle() {
  for (int i = 0; i < strands.size(); i++) {
    const uint8_t brightness = (sin(millis()/1000.0 + i / 10.0) + 1.0)/2.0 * 256;
    if (i % 2 == 0) {
      strands[i].brightness_a = brightness;
      strands[i].brightness_b = 255-brightness;
    } else {
      strands[i].brightness_b = brightness;
      strands[i].brightness_a = 255-brightness;
    }
  }
}

void twinkle2() {
  float sinValue = sin(millis()/1000.0);
  for (int i = 0; i < strands.size(); i++) {
    strands[i].brightness_a = sinValue > 0 ? sinValue * 255 : 0;
    strands[i].brightness_b = sinValue > 0 ? 0 : sinValue * -255;
  }
}

void twinkle3() {
  for (int i = 0; i < strands.size(); i++) {
    const uint8_t brightness = (sin(millis()/1000.0 + i * 2 * 3.14 * 5.0 / 9.0) + 1.0)/2.0 * 256;
    strands[i].brightness_a = brightness;
    strands[i].brightness_b = 255-brightness;
  }
}

