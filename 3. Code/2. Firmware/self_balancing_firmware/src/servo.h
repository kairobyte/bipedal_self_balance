#include <ESP32Servo.h>

extern const int left_up_pin;
extern const int right_up_pin;
extern const int right_low_pin;
extern const int left_low_pin;

extern Servo left_up;
extern Servo right_up;
extern Servo right_low;
extern Servo left_low;

void servo_setup();
void servo_move(int, int, int, int);