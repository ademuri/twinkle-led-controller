#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <iomanip>
#include <periodic-runner.h>
#include <string>

#include "constants.h"
#include "html.h"

enum class Mode {
  kBright,
  kDim,
  kTwinkle,
  kOff,
};

AsyncWebServer server(80);
PeriodicRunner runner;

Mode mode = Mode::kOff;

static const char* kMdnsName = "twinkle-controller";
static const uint32_t kRefreshMdnsDelay = 60 * 1000;

void setup() {
  Serial.begin(115200);
  Serial.println("Booted.");
  delay(500);
  Serial.println("");

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
}

int32_t write_nonvolatile_at = -1;
void loop() {
  ArduinoOTA.handle();
  runner.Run();

}

