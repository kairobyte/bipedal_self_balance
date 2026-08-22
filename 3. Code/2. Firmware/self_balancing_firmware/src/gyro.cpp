#include <Arduino.h>
#include "gyro.h"

const int MPU_ADDR_1 = 0x69; //6050
const int MPU_ADDR_2 = 0x68; //9265

// MPU6050
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H1 0x3B
#define ACCEL_XOUT_L1 0x3C

// MPU9265
#define PWR_MGMT_2 0x6B
#define ACCEL_XOUT_H2 0x3B
#define ACCEL_XOUT_L2 0x3C

int16_t X_out1;
int16_t X_out2;
float AX;

int counter = 1;
int avg1 = 0;
int avg2 = 0;

// Set these correction factors to 0 when calculating correction factors
const int16_t KF_AX1 = - 652;
const int16_t KF_AX2 =  8663 ;

// @CodeScene(disable:"Excess Number of Function Arguments")
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void calcCorrectionFactor() {
	avg1 = (avg1 + X_out1) / 2;
	avg2 = (avg2 + X_out2) / 2;
	Serial.printf("Average 1 = %.5d \t Average 2 = %.5d \n", avg1, avg2);
	counter += 1; 
	if (counter == 100) {
		while(true){
		}
	}
}

void displayResult(int16_t val) {
	
	// Serial.printf("MPU1: %.5d MPU2: %.5d \n", X_out1, X_out2);
	Serial.print("Min:");
	Serial.print(-90);
	Serial.print(",");
	Serial.print("Max:");
	Serial.print(90);
	Serial.print(",");
	Serial.print("Actual:");
	Serial.println(val);
}

void mpu_setup() {
	Wire.begin();
	delay(100);

	Wire.beginTransmission(MPU_ADDR_1);
	Wire.write(PWR_MGMT_1);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.beginTransmission(MPU_ADDR_2);
	Wire.write(PWR_MGMT_2);
	Wire.write(0x00);
	Wire.endTransmission();
}

float get_AX() {
	Wire.beginTransmission(MPU_ADDR_1);
	Wire.write(ACCEL_XOUT_H1);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU_ADDR_1, 2);
	if (Wire.available() >= 2) {
		uint8_t x_h = Wire.read();
		uint8_t x_l = Wire.read();
		X_out1 = (int16_t)((x_h << 8) | x_l) + KF_AX1;
	}

	Wire.beginTransmission(MPU_ADDR_2);
	Wire.write(ACCEL_XOUT_H2);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU_ADDR_2, 2);
	if (Wire.available() >= 2) {
		uint8_t x_h = Wire.read();
		uint8_t x_l = Wire.read();
		X_out2 = (int16_t)((x_h << 8) | x_l) + KF_AX2;

	}

	// calcCorrectionFactor();
	
	X_out1 = floatMap((float)X_out1, -16384, 16384, 90, -90);
	X_out2 = floatMap((float)X_out2, -16384, 16384, 90, -90);
	AX = (X_out1 + X_out2)/2.0f;
	return AX;
}