#include <Arduino.h>
#line 1 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "Pavankumar_5G";
const char* password = "Pavan7202@";

#define LED_PIN 1
#define BUTTON_PIN 5
#define CURRENT_VERSION "1.0.3"   // 🔹 Change this for every new firmware release

// 🔹 GitHub raw file URLs
const char* version_url  = "https://github.com/Pavankumarsp02/ESP_OTA/raw/refs/heads/main/Version.txt";
const char* firmware_url = "https://github.com/Pavankumarsp02/ESP_OTA/raw/refs/heads/main/sketch_aug25f.ino.bin";

unsigned long previousMillis = 0;
bool ledState = HIGH;

// Blink intervals
unsigned long onTime = 10000;
unsigned long offTime = 10000;

#line 23 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
void performOTA();
#line 57 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
void checkForUpdate();
#line 84 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
void setup();
#line 100 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
void loop();
#line 23 "C:\\Users\\pavan\\OneDrive\\Desktop\\ESP32_git_ota\\ESP32_git_ota.ino"
void performOTA() {
  WiFiClient client;
  HTTPClient http;

  Serial.println("Fetching new firmware...");

  http.begin(client, firmware_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();

    if (Update.begin(contentLength)) {
      size_t written = Update.writeStream(*stream);

      if (written == contentLength) {
        Serial.println("Update successful, rebooting...");
        if (Update.end(true)) {
          ESP.restart();
        }
      } else {
        Serial.printf("Written only: %d/%d. Update failed!\n", written, contentLength);
        Update.end();
      }
    } else {
      Serial.println("Not enough space for OTA update!");
    }
  } else {
    Serial.printf("Firmware HTTP error: %d\n", httpCode);
  }
  http.end();
}

void checkForUpdate() {
  WiFiClient client;
  HTTPClient http;

  Serial.println("Checking version...");

  http.begin(client, version_url);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String newVersion = http.getString();
    newVersion.trim();

    Serial.printf("Current version: %s, Latest version: %s\n", CURRENT_VERSION, newVersion.c_str());

    if (newVersion != CURRENT_VERSION) {
      Serial.println("New version available, starting OTA...");
      performOTA();
    } else {
      Serial.println("Already up to date.");
    }
  } else {
    Serial.printf("Version check HTTP error: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button = HIGH normally, LOW when pressed

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Blink LED
  unsigned long currentMillis = millis();
  if (ledState == HIGH && (currentMillis - previousMillis >= onTime)) {
    ledState = LOW;
    previousMillis = currentMillis;
    digitalWrite(LED_PIN, ledState);
  }
  else if (ledState == LOW && (currentMillis - previousMillis >= offTime)) {
    ledState = HIGH;
    previousMillis = currentMillis;
    digitalWrite(LED_PIN, ledState);
  }

  // 🔹 Check button press for OTA trigger
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Button pressed → checking for update...");
    checkForUpdate();
    delay(2000); // debounce + avoid multiple triggers
  }
}

