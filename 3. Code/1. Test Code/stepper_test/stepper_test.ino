#include <AccelStepper.h>

// #define dir_pin 12
// #define step_pin 13

#define dir_pin 32
#define step_pin 33

int speed = -100;

AccelStepper stepper(AccelStepper::DRIVER, step_pin, dir_pin);

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(2000);
  stepper.setSpeed(speed);
  Serial.println("Setup Done.");
}

void loop() {
  if(Serial.available()>0) {
    char read = Serial.read();
    if (read == 'f'){
      speed = 1000;
      Serial.println("Speed = 1000");
    } else if (read == 's') {
      speed = 100;
      Serial.println("Speed = 100");
    } else if (read == 'r'){
      speed *= -1;
      Serial.println("Speed Reversed");
    } else if (read == 'm') {
      speed = 2000;
      Serial.println("Speed = 2000");
    }
  }
  stepper.setSpeed(speed);
  stepper.runSpeed();
}