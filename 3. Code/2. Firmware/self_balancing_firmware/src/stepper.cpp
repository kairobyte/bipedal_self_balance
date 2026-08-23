#include "stepper.h"

#define dir_pin_l 12
#define step_pin_l 13

#define dir_pin_r 32
#define step_pin_r 33

int init_speed = 1000;

AccelStepper stepper_left(AccelStepper::DRIVER, step_pin_l, dir_pin_l);
AccelStepper stepper_right(AccelStepper::DRIVER, step_pin_r, dir_pin_r);

void stepper_setup() {
	stepper_left.setMaxSpeed(1600);
	stepper_right.setMaxSpeed(1600);
	stepper_left.setSpeed(init_speed);
	stepper_right.setSpeed(init_speed);
}

void runStepper(int speed){
	stepper_left.setSpeed(-speed);
	stepper_right.setSpeed(speed);
	stepper_left.runSpeed();
	stepper_right.runSpeed();
} 