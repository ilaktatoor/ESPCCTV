#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "esp_camera.h"

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camara_pins.h"

// Default SoftAP credentials
const char* softAP_ssid = "ESP32_Config_AP";
const char* softAP_password = "rootwizz";

// Web server object
AsyncWebServer server(80);

// Preferences object for storing settings
Preferences preferences;

// Wi-Fi credentials variables (will be set by the user)
String ssid = ""; // Wi-Fi SSID
String password = ""; // Wi-Fi password
bool connectWiFi = false; // Whether Wi-Fi connection is enabled or not
bool softAPEnabled = true; // SoftAP state

// Initialize server URL as empty (user will input this)
String serverUrl = ""; 
String port = "5050"; 
const char* endPoint = "/video-frame";

// HTTPClient object (initialized once)
HTTPClient http;

// Function Declarations
void saveSettings(AsyncWebServerRequest *request);
void connectWiFiHandler(AsyncWebServerRequest *request);

// Function to send frames to the server
void sendFrameToServer() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  if (serverUrl == "" || port == "") {
    Serial.println("Server URL or port is not set");
    esp_camera_fb_return(fb);
    return;
  }

  String mainUrl = "http://" + serverUrl + ":" + port + endPoint;
  Serial.println("Sending request to:");
  Serial.print("IP: ");
  Serial.println(serverUrl);
  Serial.print("Port: ");
  Serial.println(port);
  Serial.print("Endpoint: ");
  Serial.println(endPoint);
  
  Serial.println("Sending the frame...");
  http.begin(mainUrl);
  http.addHeader("Content-Type", "image/jpeg");

  int httpResponseCode = http.POST(fb->buf, fb->len);

  if (httpResponseCode > 0) {
    Serial.printf("Response: %s\n", http.getString().c_str());
  } else {
    Serial.printf("Error sending frame: %d\n", httpResponseCode);
  }

  http.end();
  esp_camera_fb_return(fb);
  yield(); // Allow the system to perform background tasks
}

// Login page
String loginPage() {
  return "<html><body><h1>Login</h1>"
         "<form action='/login' method='POST'>"
         "Username: <input type='text' name='username'><br>"
         "Password: <input type='password' name='password'><br>"
         "<input type='submit' value='Login'>"
         "</form></body></html>";
}

// Config page with Connect Wi-Fi and Save buttons
String configPage() {
  String switchState = softAPEnabled ? "checked" : "";
  return "<html><body><h1>Configuration</h1>"
         "<form action='/save' method='POST'>"
         "Wi-Fi SSID: <input type='text' name='ssid' value='" + ssid + "'><br>"
         "Wi-Fi Password: <input type='password' name='password' value='" + password + "'><br>"
         "Server URL: <input type='text' name='serverUrl' value='" + serverUrl + "'><br>"
         "Port: <input type='text' name='port' value='" + port + "'><br>"
         "Enable SoftAP: <input type='checkbox' name='softAPEnabled' " + switchState + "><br>"
         "<input type='submit' value='Save Settings'><br><br>"
         "</form>"
         "<form action='/connectWiFi' method='POST'>"
         "<input type='submit' value='Connect to Wi-Fi'>"
         "</form>"
         "</body></html>";
}

// New route to handle Wi-Fi connection
void connectWiFiHandler(AsyncWebServerRequest *request) {
  // Attempt to connect to the specified Wi-Fi network
  Serial.println("Attempting to connect to Wi-Fi...");
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long startAttemptTime = millis();
  bool connected = false;

  while (millis() - startAttemptTime < 50000) { // 50-second timeout
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    delay(100); // Allow other tasks to run and prevent watchdog timeout
    Serial.print(".");
    yield(); // Yield to prevent watchdog reset
  }

  if (connected) {
    Serial.println("\nConnected to Wi-Fi successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    connectWiFi = true;  // Set this flag to true once connected
    request->send(200, "text/html", "<html><body><h1>Wi-Fi Connected</h1><a href='/config'>Go to Config Page</a></body></html>");
  } else {
    Serial.println("\nFailed to connect to Wi-Fi. Please check your credentials.");
    request->send(200, "text/html", "<html><body><h1>Wi-Fi Connection Failed</h1><a href='/config'>Go to Config Page</a></body></html>");
  }
}

// Save settings route
void saveSettings(AsyncWebServerRequest *request) {
  ssid = request->arg("ssid");
  password = request->arg("password");
  serverUrl = request->arg("serverUrl");
  port = request->arg("port");
  String softAPState = request->arg("softAPEnabled");

  softAPEnabled = (softAPState == "on");

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.putString("serverUrl", serverUrl);
  preferences.putString("port", port);
  preferences.putBool("softAPEnabled", softAPEnabled);

  // Manage SoftAP
  if (softAPEnabled) {
    WiFi.softAP(softAP_ssid, softAP_password);
    Serial.println("SoftAP started.");
  } else {
    WiFi.softAPdisconnect(true);
    Serial.println("SoftAP disabled.");
  }

  request->send(200, "text/html", "<html><body><h1>Settings Saved</h1><a href='/config'>Go to Config Page</a></body></html>");
}

// Setup function
void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize the camera
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

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Load stored settings
  preferences.begin("config", false);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  serverUrl = preferences.getString("serverUrl", "");
  port = preferences.getString("port", "5050");
  softAPEnabled = preferences.getBool("softAPEnabled", true);

  // Start SoftAP
  if (softAPEnabled) {
    WiFi.softAP(softAP_ssid, softAP_password);
    Serial.println("SoftAP started");
  }

  // Define server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", loginPage());
  });

  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request) {
    String username = request->arg("username");
    String password = request->arg("password");
    if (username == "rootatkali" && password == "rootwizz") {
      request->send(200, "text/html", "<html><body><h1>Login Successful</h1><a href='/config'>Go to Config Page</a></body></html>");
    } else {
      request->send(401, "text/html", "<html><body><h1>Login Failed</h1><a href='/'>Back to Login</a></body></html>");
    }
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", configPage());
  });

  server.on("/save", HTTP_POST, saveSettings);

  // New route for Wi-Fi connection
  server.on("/connectWiFi", HTTP_POST, connectWiFiHandler);

  // Start the server
  server.begin();
}

void loop() {
  if (connectWiFi && WiFi.status() == WL_CONNECTED) {
    sendFrameToServer();
    delay(1000); // Add a small delay
  }
  yield(); // Keep the system alive and responsive
}
