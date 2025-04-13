#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <EEPROM.h>
WebServer    server(80);

#define CAMERA_MODEL_AI_THINKER
#include "camara_pins.h"

#define CAMERA_ID "cam1"  // change this per device


struct settings {
  char ssid[30];
  char password[30];
  char camid[10];
  char server_host[64];  // IP or domain
  uint16_t port;
} user_wifi = {};


const char* ssid = "INFINITUM7AAF";
const char* password = "amZuP72VhZ";

String serverUrl = String("http://192.168.1.100:5050/video-frame/") + CAMERA_ID;

void setup() {
  EEPROM.begin(sizeof(struct settings) );
  EEPROM.get( 0, user_wifi );

  Serial.begin(115200);
  Serial.printf("Booting %s...\n", CAMERA_ID);

  if (!initCamera()) {
    Serial.println("Camera init failed");
    return;
  }

 WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);
  
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 30) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Setup Portal", "mrdiy.ca");
      break;
    }
  }
  server.on("/",  handlePortal);
  server.begin();

  Serial.println();
  Serial.println("WiFi connected");
  Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    sendFrameToServer();
    delay(100);
  }
}

