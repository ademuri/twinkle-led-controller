#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <iomanip>
#include <periodic-runner.h>
#include <string>
#include <vector>

#include "constants.h"
#include "html.h"

enum class Mode {
  kBright,
  kDim,
  kTwinkle,
  kOff,
};

struct Strand {
  uint8_t brightness_a;
  uint8_t brightness_b;

  const int pin_0;
  const int pin_1;
};

// TODO: does this need to be volatile?
static std::vector<Strand> strands = {
  {0, 0, 22, 23},
  {0, 0, 19, 21},
  {0, 0, 17, 18},
  {0, 0, 4, 16},
  // Note: 4, 5, 6 don't work (flash pins)
  {0, 0, 27, 14},
  {0, 0, 25, 26},
  {0, 0, 32, 33},
};

AsyncWebServer server(80);
PeriodicRunner runner;

Mode mode = Mode::kOff;

static const char* kMdnsName = "twinkle-controller";
static const uint32_t kRefreshMdnsDelay = 60 * 1000;

// Timer stuff, used for PWM. See the ESP32 timer example:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/Timer/RepeatTimer/RepeatTimer.ino
// Used to disable task switching during ISR
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = nullptr;

void IRAM_ATTR onTimer() {
  static uint16_t pwm_counter = 0;
  static uint8_t alternator = 0;

  portENTER_CRITICAL_ISR(&timerMux);
  for (auto strand : strands) {
    if (alternator == 0) {
      if (strand.brightness_a > pwm_counter) {
        digitalWrite(strand.pin_0, HIGH);
        digitalWrite(strand.pin_1, LOW);
      } else {
        digitalWrite(strand.pin_0, LOW);
        digitalWrite(strand.pin_1, LOW);
      }
      alternator = 1;
    } else {
      if (strand.brightness_b > pwm_counter) {
        digitalWrite(strand.pin_0, LOW);
        digitalWrite(strand.pin_1, HIGH);
      } else {
        digitalWrite(strand.pin_0, LOW);
        digitalWrite(strand.pin_1, LOW);
      }
      alternator = 0;
    }
  }
  if (alternator == 1) {
    pwm_counter++;
    if (pwm_counter > 255) {
      pwm_counter = 0;
    }
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booted.");
  delay(500);
  Serial.println("");

  for (auto strand : strands) {
    pinMode(strand.pin_0, OUTPUT);
    pinMode(strand.pin_1, OUTPUT);
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
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();

  if (MDNS.begin(kMdnsName)) {
    // Add service to MDNS-SD
    MDNS.addService("_http", "_tcp", 80);
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  runner.Add(kRefreshMdnsDelay, []() {
    if (!MDNS.begin(kMdnsName)) {
      Serial.println("Error refreshing MDNS responder!");
    }
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  });
  // Note: use POST, not PUT, so that we don't have to include the regex
  // library for URL matching, which is large
  server.on("/bright", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kBright;
  });
  server.on("/dim", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kDim;
  });
  server.on("/twinkle", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kTwinkle;
  });
  server.on("/off", HTTP_POST, [](AsyncWebServerRequest *request) {
    mode = Mode::kOff;
  });

  server.begin();
  Serial.println("Started server.");

  timer = timerBegin(0, 60, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // Call every 1ms
  timerAlarmWrite(timer, 8, /* repeat */ true);
  timerAlarmEnable(timer);
}

int32_t write_nonvolatile_at = -1;
void loop() {
  ArduinoOTA.handle();
  // TODO: re-enable if timing works
  runner.Run();

  for (int i = 0; i < 255; i++) {
    strands[0].brightness_a = i;
    strands[0].brightness_b = 255 - i;
    delay(10);
  }
  for (int i = 255; i > 0; i--) {
    strands[0].brightness_a = i;
    strands[0].brightness_b = 255 - i;
    delay(10);
  }

  Serial.println(millis());
}

