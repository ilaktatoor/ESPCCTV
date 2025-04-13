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
} user_wifi = {};

const char* ssid = "INFINITUM7AAF";
const char* password = "amZuP72VhZ";

String serverUrl = String("http://192.168.1.100:5050/video-frame/") + CAMERA_ID;

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  return esp_camera_init(&config) == ESP_OK;
}

void sendFrameToServer() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);
  if (httpResponseCode > 0) {
    Serial.printf("Response: %s\n", http.getString().c_str());
  } else {
    Serial.printf("Error sending frame: %d\n", httpResponseCode);
  }

  http.end();
  esp_camera_fb_return(fb);
}

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

