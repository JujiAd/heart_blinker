#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ElegantOTA.h>

#define LED 5

// Replace with your network credentials
const char *ssid = "SpyCats";
const char *password = "Felix&Jiji";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char *PARAM_INPUT_1 = "blink";

// Variables to save values from HTML form
bool blinkRequest = false;

// Initialize LittleFS
void initLittleFS()
{
  if (!LittleFS.begin(true))
  {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else
  {
    Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Reconnect WiFi
void reconnectWiFi()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(1000);
  }
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  initWiFi();
  initLittleFS();
  pinMode(LED, OUTPUT);

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.serveStatic("/", LittleFS, "/");

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    int params = request->params();
    Serial.print("Number of input params: ");
    Serial.println(String(params));

    for(int i=0;i<params;i++){
      const AsyncWebParameter* p = request->getParam(i);
      Serial.printf("Parameter: %s, %s\n", p->name().c_str(), p->value().c_str());

      // HTTP POST blink request
      if (p->name() == PARAM_INPUT_1) {
        blinkRequest = true;
      }
    }
    request->send(LittleFS, "/index.html", "text/html"); });

  server.begin();
  ElegantOTA.begin(&server);
}



void loop()
{
  // Verify that we are still connected to wifi
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }

  // Start ElegantOTA for remote updates
  ElegantOTA.loop();

  // Handle blinkRequest
  if (blinkRequest)
  {
    blink();
    blinkRequest = false;
  }
}