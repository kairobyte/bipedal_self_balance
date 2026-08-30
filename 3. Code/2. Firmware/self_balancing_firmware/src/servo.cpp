#include "servo.h"

const int left_up_pin = 18;
const int right_up_pin = 16;
const int right_low_pin = 2;
const int left_low_pin = 4;

Servo left_up; //LU
Servo right_up; //RU
Servo right_low; //RL
Servo left_low; //LL

void servo_setup(){
	left_up.attach(left_up_pin, 544, 2400); 
	right_up.attach(right_up_pin, 544, 2400);
	right_low.attach(right_low_pin, 544, 2400);
	left_low.attach(left_low_pin, 544, 2400);
	// left_low.attach(left_low_pin, 544, 2400);
}

void servo_move(int a, int b, int c, int d){
	left_up.write(a);
	right_up.write(b);
	right_low.write(c);
	left_low.write(d);
}

// default pose = 55, 45, 40, 140