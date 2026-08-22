#include <ESP32Servo.h>

static const int servoPin1 = 18;
static const int servoPin2 = 16;
static const int servoPin3 = 2;
static const int servoPin4 = 4;

Servo servo1; //LU
Servo servo2; //RU
Servo servo3; //RL
Servo servo4; //LL

void setup() {
  delay(1000);
  Serial.begin(115200);
  servo1.attach(servoPin1, 544, 2400); 
  servo2.attach(servoPin2, 544, 2400);
  servo3.attach(servoPin3, 544, 2400);
  servo4.attach(servoPin4, 544, 2400);
}

void loop(){
  servo1.write(55);
  servo2.write(45);
  servo3.write(40);
  servo4.write(140);
  delay(2000);
}