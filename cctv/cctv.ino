#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <EEPROM.h>
WebServer server(80);

#define CAMERA_MODEL_AI_THINKER
#include "camara_pins.h"


#define RESET_PIN 16  // Typically GPIO16 is the boot button on AI Thinker

struct settings {
  char ssid[30];
  char password[30];
  char camid[10];
  char server_host[64];  // IP or domain
  uint16_t port;


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

void setup() {
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);

  Serial.begin(115200);
  if (!initCamera()) {
    Serial.println("Restarting due to camera failure");
    delay(1000);
    ESP.restart();

  }

  Serial.printf("Booting %s...\n", user_wifi.camid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 30) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Setup Portal", "stdevsec.com");
      break;
    }
  }
  server.on("/", handlePortal);
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
