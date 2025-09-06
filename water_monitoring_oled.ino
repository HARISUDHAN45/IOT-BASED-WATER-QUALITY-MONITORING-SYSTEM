#define BLYNK_TEMPLATE_ID "TMPL3ZCc8aKpO"
#define BLYNK_TEMPLATE_NAME "Water Monitoring"
#define BLYNK_AUTH_TOKEN "ua1ssgrzLrIwc1808VvXHnafje1iTgt-"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi credentials
char ssid[] = "Dharanishvivo";
char pass[] = "#1234@5678";

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Sensor pins
#define TURBIDITY_PIN 34
#define TDS_PIN 35

BlynkTimer timer;

// Thresholds (WHO standards approx)
#define TDS_LIMIT 500      // ppm
#define TURBIDITY_LIMIT 5  // NTU

void sendSensorData() {
  // Read raw values
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  int tdsRaw = analogRead(TDS_PIN);

  // Convert to voltages
  float turbidityVolt = turbidityRaw * (3.3 / 4095.0);
  float tdsVolt = tdsRaw * (3.3 / 4095.0);

  // TDS calculation (simple ppm approx)
  float tdsValue = (tdsVolt * 1000);

  // Turbidity using polynomial formula
  float turbidityNTU = -1120.4 * turbidityVolt * turbidityVolt + 5742.3 * turbidityVolt - 4352.9;
  if (turbidityNTU < 0) turbidityNTU = 0;  // prevent negative values

  // Condition check
  bool drinkable = (tdsValue < TDS_LIMIT && turbidityNTU < TURBIDITY_LIMIT);

  // -------- Serial Monitor --------
  Serial.print("TDS (ppm): ");
  Serial.print(tdsValue);
  Serial.print(" | Turbidity (NTU): ");
  Serial.print(turbidityNTU);
  Serial.print(" --> ");
  if (drinkable) {
    Serial.println("DRINKABLE");
  } else {
    Serial.println("NOT DRINKABLE");
  }

  // -------- OLED Display --------
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("TDS: ");
  display.print(tdsValue, 1);
  display.println(" ppm");

  display.print("Turbidity: ");
  display.print(turbidityNTU, 1);
  display.println(" NTU");

  display.setTextSize(2);
  display.setCursor(0, 40);
  if (drinkable) {
    display.println("DRINKABLE");
  } else {
    display.println("NOT DRINKABLE");
  }
  display.display();

  // -------- Blynk Data --------
  Blynk.virtualWrite(V0, tdsValue);
  Blynk.virtualWrite(V1, turbidityNTU);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Water Monitor");
  display.display();
  delay(2000);

  // Timer for sending data
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}