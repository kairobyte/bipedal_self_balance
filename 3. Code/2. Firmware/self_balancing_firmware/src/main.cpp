#include <Arduino.h>
#include "stepper.h"
#include "led.h"
#include "servo.h"
#include "gyro.h"    
#include "tuning.h"

const float SERVO_BASE_1 = servo1; 
const float SERVO_BASE_2 = servo2;

float command = 0;
float integral = 0;
float lastAngle = SET_POINT;
float servoChange = 0.0;

unsigned long lastControlTime = 0;
unsigned long last_time = 0;

void setup() {
	Serial.begin(115200);
	mpu_setup();
	stepper_setup();
	servo_setup();
	rgb_setup();
	tuning_setup();

	// servo_move(55, 45, 40, 140);
	servo_move(servo1, servo2, servo3, servo4);
	set_rgb(0, 255, 0);
	Serial.println("Setup Done.");
}

// @CodeScene(disable:"Complex Method")
void loop() {
	unsigned long now = micros();
	tuning_poll();

	if (now - lastControlTime >= (unsigned long)CONTROL_INTERVAL_US) {
		float dt = (now - lastControlTime) / 1000000.0f;
		lastControlTime = now;
		if (dt <= 0) dt = (unsigned long)CONTROL_INTERVAL_US / 1000000.0f;

		float angle = get_angle();
		float error = angle - SET_POINT;
		if (fabs(error) < DEADBAND) error = 0;

		integral += error * dt;
		integral = constrain(integral, -50.0f, 50.0f);

		float derivative = (angle - lastAngle) / dt;
		lastAngle = angle;

		float sign = (error > 0) ? 1.0f : (error < 0 ? -1.0f : 0.0f);
		float pTerm = kp * error + kp_boost * error * error * sign;
		command = pTerm + ki * integral - kd * derivative; // note: + kd now

		if (error != 0 && fabs(command) > 0 && fabs(command) < MIN_SPEED) {
			command = (command > 0) ? MIN_SPEED : -MIN_SPEED;
		} else if (error == 0) {
			command = 0;
		}
		command = constrain(command, -MAX_COMMAND, MAX_COMMAND);
		setStepperSpeed((int)command);

		float legLean = constrain(LEAN_GAIN * error, -LEAN_LIMIT, LEAN_LIMIT);
		float newServo1 = constrain(servo1 + legLean, 0, 180);
		float newServo2 = constrain(servo2 + legLean, 0, 180);
		servo_move(newServo1, newServo2, servo3, servo4);
		// Serial.print("Angle Max=");
		// Serial.print(90);
		// Serial.print(",");
		// Serial.print("Angle Min");
		// Serial.print(0);
		// Serial.print(",");
		// Serial.print("Angle=");
		// Serial.print(angle);
		// Serial.print(",");
		// Serial.print("SetPoint=");
		// Serial.print(SET_POINT);
		// Serial.print(",");
		// Serial.print("Error");
		// Serial.print(error);
		// Serial.print(",");
		// Serial.print("Command=");
		// Serial.print(command);
		// Serial.println();
	}

	stepStepper();
}