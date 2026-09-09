// tuning.h
#pragma once
#include <Arduino.h>

extern volatile float ke, kc;
extern volatile float kp, ki, kd, kp_boost;
extern volatile float SET_POINT, DEADBAND, acelk, gyrok, beta;
extern volatile float MAX_COMMAND, MIN_SPEED;
extern volatile float servo1, servo2, servo3, servo4;
extern volatile float LEAN_GAIN, LEAN_LIMIT;
extern volatile float CONTROL_INTERVAL_US;

void tuning_setup();    
void tuning_poll();       