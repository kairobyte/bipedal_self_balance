#include <Arduino.h>
#include "stepper.h"
#include "led.h"
#include "servo.h"
#include "gyro.h"

float filteredAX = 0;
const float ALPHA = 0.02f; 
const float DEADBAND = 0.75f;
const float GAIN = 75.0f;      


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
	float raw = get_AX();
	filteredAX = ALPHA * raw + (1 - ALPHA) * filteredAX;
	float command = 0;
	if (fabs(filteredAX) > DEADBAND) {
		command = GAIN * filteredAX;
	}

	runStepper((long)command);
	Serial.println(filteredAX, 5);
}