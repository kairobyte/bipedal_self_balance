#pragma once

#include <AccelStepper.h>

extern AccelStepper stepper_left;
extern AccelStepper stepper_right;
extern int init_speed;

void stepper_setup();
void runStepper(int speed);
void setStepperSpeed(int speed);
void stepStepper(void);