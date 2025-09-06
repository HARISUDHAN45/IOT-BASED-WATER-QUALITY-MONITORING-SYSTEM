#define BLYNK_TEMPLATE_ID "TMPL3ZCc8aKpO"
#define BLYNK_TEMPLATE_NAME "Water Monitoring"
#define BLYNK_AUTH_TOKEN "ua1ssgrzLrIwc1808VvXHnafje1iTgt-"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Dharanishvivo";
char pass[] = "#1234@5678";

// Sensor pins
#define TURBIDITY_PIN 34
#define TDS_PIN 35

// Motor pins (L298N with ESP32)
#define MOTOR1_IN1 26   // Motor 1 control pin
#define MOTOR1_IN2 27
#define MOTOR2_IN1 32   // Motor 2 control pin
#define MOTOR2_IN2 33

BlynkTimer timer;

// Thresholds (WHO standards approx)
#define TDS_LIMIT 500      // ppm
#define TURBIDITY_LIMIT 5  // NTU

void runMotor1() {
  // Run Motor 1 forward
  digitalWrite(MOTOR1_IN1, HIGH);
  digitalWrite(MOTOR1_IN2, LOW);

  // Stop Motor 2
  digitalWrite(MOTOR2_IN1, LOW);
  digitalWrite(MOTOR2_IN2, LOW);
}

void runMotor2() {
  // Run Motor 2 forward
  digitalWrite(MOTOR2_IN1, HIGH);
  digitalWrite(MOTOR2_IN2, LOW);

  // Stop Motor 1
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, LOW);
}

void stopMotors() {
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, LOW);
  digitalWrite(MOTOR2_IN1, LOW);
  digitalWrite(MOTOR2_IN2, LOW);
}

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
    Serial.println("DRINKABLE → Motor 1 ON (Village)");
    runMotor1();
  } else {
    Serial.println("NOT DRINKABLE → Motor 2 ON (Source)");
    runMotor2();
  }

  // -------- Blynk Data --------
  Blynk.virtualWrite(V0, tdsValue);
  Blynk.virtualWrite(V1, turbidityNTU);
  Blynk.virtualWrite(V2, drinkable ? 1 : 0); // 1=Drinkable, 0=Not drinkable
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Motor pins setup
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);

  stopMotors();

  // Timer for sending data
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
