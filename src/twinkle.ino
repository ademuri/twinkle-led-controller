#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <iomanip>
#include <rom/rtc.h>
#include <string>
#include <vector>

#include "constants.h"
#include "html.h"

enum class Mode {
  kBright,
  kDim,
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

AsyncWebServer server(80);

Mode mode = Mode::kOff;

static const char* kMdnsName = "twinkle-controller";
static const uint32_t kRefreshMdnsDelay = 60 * 1000;

// Timer stuff, used for strand control. See the ESP32 timer example:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Timer/RepeatTimer/RepeatTimer.ino
// Used to disable task switching during ISR
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = nullptr;

static const double kPwmFreq = 78125;
static const uint8_t kPwmResolution = 8;

static const int kLedPin = 12;

void IRAM_ATTR onTimer() {
  static uint16_t pwm_counter = 0;
  static uint8_t alternator = 0;

  portENTER_CRITICAL_ISR(&timerMux);
  for (uint32_t i = 0; i < strands.size(); i++) {
    Strand strand = strands[i];
    if (alternator == 0) {
      ledcDetachPin(strand.pin_1);
      digitalWrite(strand.pin_1, LOW);
      ledcWrite(i, strand.brightness_a);
      ledcAttachPin(strand.pin_0, i);
    } else {
      ledcDetachPin(strand.pin_0);
      digitalWrite(strand.pin_0, LOW);
      ledcWrite(i, strand.brightness_b);
      ledcAttachPin(strand.pin_1, i);
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

  portEXIT_CRITICAL_ISR(&timerMux);
}

String htmlTemplateProcessor(const String &var) {
  if (var == "RESET") {
    return String(rtc_get_reset_reason(0)) + String(" ") + String(rtc_get_reset_reason(1));
  }
  return String();
}

void setup() {
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, HIGH);
  Serial.begin(115200);
  Serial.println("Booted.");
  Serial.println("");

  for (uint32_t i = 0; i < strands.size(); i++) {
    Strand strand = strands[i];
    pinMode(strand.pin_0, OUTPUT);
    pinMode(strand.pin_1, OUTPUT);
    ledcSetup(i, kPwmFreq, kPwmResolution);
    ledcAttachPin(strand.pin_0, i);
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
      timerAlarmDisable(timer);

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
      timerAlarmEnable(timer);

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
    for (int i = 0; i < strands.size(); i++) {
        strands[i].brightness_a = 255;
        strands[i].brightness_b = 255;
    }
    mode = Mode::kBright;
  });
  server.on("/dim", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kDim;
  });
  server.on("/twinkle", HTTP_POST, [](AsyncWebServerRequest *request) {
    for (int j = 0; j < strands.size(); j++) {
      strands[j].brightness_a = 0;
      strands[j].brightness_b = 0;
    }
    mode = Mode::kTwinkle;
  });
  server.on("/off", HTTP_POST, [](AsyncWebServerRequest *request) {
    for (int j = 0; j < strands.size(); j++) {
      strands[j].brightness_a = 0;
      strands[j].brightness_b = 0;
    }
    mode = Mode::kOff;
  });

  mode = Mode::kHalf;

  server.begin();
  Serial.println("Started server.");
  digitalWrite(kLedPin, LOW);

  timer = timerBegin(0, 240, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // Call at 10kHz (?)
  timerAlarmWrite(timer, 100, /* repeat */ true);
  timerAlarmEnable(timer);
}

void loop() {
  ArduinoOTA.handle();

  switch(mode) {
    case Mode::kTwinkle:
      twinkle();
      break;
    case Mode::kHalf:
        for (int j = 0; j < strands.size(); j++) {
          strands[j].brightness_b = 255;
        }
      break;
    }
}

void twinkle() {
  const uint8_t brightness = (sin(millis()/1000.0) + 1.0)/2.0 * 255;
  for (int i = 0; i < strands.size(); i++) {
    strands[i].brightness_a = brightness;
    strands[i].brightness_b = 255-brightness;
  }
}

void twinkle2() {
  float sinValue = sin(millis()/1000.0);
  for (int i = 0; i < strands.size(); i++) {
    strands[i].brightness_a = sinValue > 0 ? sinValue * 255 : 0;
    strands[i].brightness_b = sinValue > 0 ? 0 : sinValue * -255;
  }
}

