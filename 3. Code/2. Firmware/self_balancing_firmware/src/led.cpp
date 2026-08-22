#include <Arduino.h>
#include "led.h"

int redPin = 25;
int greenPin = 26;
int bluePin = 27;

void rgb_setup(){
	pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
}

void set_rgb(int r, int g, int b){
	analogWrite(redPin, r);
	analogWrite(greenPin, g);
	analogWrite(bluePin, b);
}