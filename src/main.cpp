#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// -------- WiFi Credentials --------
const char* ssid = "Robotics Institute of Kenya";
const char* password = "RObOT1C&#357";

// -------- Flask server endpoint --------
String serverURL = "http://192.168.1.56:5000/api/ds18b20";

// -------- OLED settings --------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------- DS18B20 Sensor --------
#define ONE_WIRE_PIN 7     // DS18B20 data pin on ESP32-C3 mini
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);

  // ----- Connect WiFi -----
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  // ----- Initialize OLED -----
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.display();
  delay(1000);

  // ----- Initialize DS18B20 -----
  sensors.begin();
}

void loop() {
  // ----- Read DS18B20 -----
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float tempF = sensors.toFahrenheit(tempC);
  float tempK = tempC + 273.15;

  // ----- Serial Output -----
  Serial.printf("TempC: %.2f°C  TempF: %.2f°F  TempK: %.2fK\n", tempC, tempF, tempK);

  // ----- Update OLED -----
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("DS18B20 Sensor");

  display.setCursor(0, 15);
  display.printf("TempC: %.2f C", tempC);

  display.setCursor(0, 30);
  display.printf("TempF: %.2f F", tempF);

  display.setCursor(0, 45);
  display.printf("TempK: %.2f K", tempK);

  display.display();

  // ----- Send to MongoDB through Flask -----
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Build JSON
    String jsonData = "{\"tempC\":" + String(tempC) +
                      ",\"tempF\":" + String(tempF) +
                      ",\"tempK\":" + String(tempK) +
                      "}";

    int httpResponseCode = http.POST(jsonData);

    Serial.print("Server Response: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      Serial.println(http.getString());
    } else {
      Serial.println("Error sending POST request");
    }

    http.end();
  }

  delay(3000);  // read every 3 seconds
}
