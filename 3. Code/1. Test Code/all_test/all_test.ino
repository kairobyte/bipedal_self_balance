#include <ESP32Servo.h>
#include <AccelStepper.h>
#include <Wire.h>

const int MPU_ADDR_1 = 0x69; //6050
const int MPU_ADDR_2 = 0x68; //9265

// Pin definitions
#define DIR_PIN 32
#define STEP_PIN 33
// #define dir_pin 12
// #define step_pin 13
static const int SERVO_PIN1 = 2;
static const int SERVO_PIN2 = 4;

// Control variables
int speedVal = -100;
unsigned long lastTime = 0;
const unsigned long interval = 2000; // 2 seconds delay
bool servoState = false;

// Hardware objects
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
Servo servo1;
Servo servo2;

// MPU6050
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H1 0x3B
#define ACCEL_XOUT_L1 0x3C

// MPU9265
#define PWR_MGMT_2 0x6B
#define ACCEL_XOUT_H2 0x3B
#define ACCEL_XOUT_L2 0x3C

int16_t X_out1;
int16_t X_out2;

int counter = 1;
int avg1 = 0;
int avg2 = 0;

// Set these correction factors to 0 when calculating correction factors
const int16_t KF_AX1 = - 652;
const int16_t KF_AX2 =  8663 ;

void displayResult() {
  X_out1 = map(X_out1, -16384, 16384, 90, -90);
  X_out2 = map(X_out2, -16384, 16384, 90, -90);
  // Serial.printf("MPU1: %.5d MPU2: %.5d \n", X_out1, X_out2);
  Serial.print("Min:");
  Serial.print(-90);
  Serial.print(",");
  Serial.print("Max:");
  Serial.print(90);
  Serial.print(",");
  Serial.print("Actual:");
  Serial.println((X_out1 + X_out2)/2);
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(100);

  servo1.attach(SERVO_PIN1);
  servo2.attach(SERVO_PIN2);
  
  stepper.setMaxSpeed(2000);
  stepper.setSpeed(speedVal);
  
  Serial.println("Setup Done.");

  delay(2000);

  Wire.beginTransmission(MPU_ADDR_1);
  Wire.write(PWR_MGMT_1);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_ADDR_2);
  Wire.write(PWR_MGMT_2);
  Wire.write(0x00);
  Wire.endTransmission();
}

void loop() {
  // 1. Non-blocking Timer for Servos
  unsigned long currentMillis = millis();
  if (currentMillis - lastTime >= interval) {
    lastTime = currentMillis;      // Reset timer anchor
    servoState = !servoState;       // Toggle state

    if (servoState) {
      servo1.write(180);
      servo2.write(180);
    } else {
      servo1.write(0);
      servo2.write(0);
    }
  }

  // 2. Serial Command Parser
  if (Serial.available() > 0) {
    char readChar = Serial.read();
    
    if (readChar == 'f') {
      speedVal = 1000;
      Serial.println("Speed = 1000");
    } else if (readChar == 's') {
      speedVal = 100;
      Serial.println("Speed = 100");
    } else if (readChar == 'r') {
      speedVal *= -1;
      Serial.println("Speed Reversed");
    } else if (readChar == 'm') {
      speedVal = 2000;
      Serial.println("Speed = 2000");
    }
    
    stepper.setSpeed(speedVal); // Update stepper speed only when changed
  }

  // 3. Keep Stepper Moving
  stepper.runSpeed();

  Wire.beginTransmission(MPU_ADDR_1);
  Wire.write(ACCEL_XOUT_H1);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR_1, 2);
  if (Wire.available() >= 2) {
    uint8_t x_h = Wire.read();
    uint8_t x_l = Wire.read();
    X_out1 = (int16_t)((x_h << 8) | x_l) + KF_AX1;
  }

  Wire.beginTransmission(MPU_ADDR_2);
  Wire.write(ACCEL_XOUT_H2);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR_2, 2);
  if (Wire.available() >= 2) {
    uint8_t x_h = Wire.read();
    uint8_t x_l = Wire.read();
    X_out2 = (int16_t)((x_h << 8) | x_l) + KF_AX2;
    
  }

  // calcCorrectionFactor();
  displayResult();
}