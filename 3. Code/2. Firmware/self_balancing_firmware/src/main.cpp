#include <Arduino.h>
#include "stepper.h"
#include "led.h"
#include "servo.h"
#include "gyro.h"

const float ALPHA = 0.02f; 
const float DEADBAND = 2.0f;
const float GAIN = 75.0f;      

const float SET_POINT = 8.0f;

const float kp = 0.0f;
const float ki = 0.0f;
const float kd = 0.0f;


void setup() {
	Serial.begin(115200);
	mpu_setup();
	stepper_setup();
	servo_setup();
	rgb_setup();

	servo_move(55, 45, 40, 140);
	set_rgb(0, 255, 0);
	Serial.println("Setup Done.");
}

void loop() {
	float angle = get_angle();
	float error = angle - SET_POINT;
	// filteredAX = ALPHA * raw + (1 - ALPHA) * filteredAX;
	float command = 0;
	if (fabs(error) > DEADBAND) {
		command = GAIN * error;
	}

	runStepper((long)command);
}