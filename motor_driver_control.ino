// Motor Driver control pins (ESP32 GPIO)
#define IN1 26   // Motor 1
#define IN2 27   // Motor 1
#define IN3 32   // Motor 2
#define IN4 33   // Motor 2
#define ENA 25   // Enable Motor 1
#define ENB 14   // Enable Motor 2

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable both motor channels
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  stopMotors(); // Ensure all motors are OFF at start
}

void loop() {
  // Motor1 ON, Motor2 OFF
  motor1On();
  motor2Off();
  delay(5000);  // 5 seconds

  // Motor2 ON, Motor1 OFF
  motor1Off();
  motor2On();
  delay(5000);  // 5 seconds
}

// Motor functions
void motor1On() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motor1Off() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void motor2On() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motor2Off() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  motor1Off();
  motor2Off();
}
