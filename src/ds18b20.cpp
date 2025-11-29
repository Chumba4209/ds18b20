#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DS18B20 on ESP32-C3 Mini → GPIO7
#define ONE_WIRE_PIN 7
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);

  // Start DS18B20
  sensors.begin();

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED init failed!");
    while (1);
  }

  display.clearDisplay();
  display.display();
  delay(1000);
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  float tempK = tempC + 273.15;

  // Print to Serial
  Serial.printf("Temp: %.2f C   %.2f K\n", tempC, tempK);

  // Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("DS18B20 Temperature");

  display.setCursor(0, 20);
  display.printf("Temp: %.2f C", tempC);

  display.setCursor(0, 40);
  display.printf("Kelvin: %.2f K", tempK);

  display.display();

  delay(2000);
}
