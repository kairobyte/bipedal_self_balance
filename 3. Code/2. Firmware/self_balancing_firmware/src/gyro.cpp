#include <Arduino.h>
#include "gyro.h"

const int MPU_ADDR_1 = 0x69; //6050
const int MPU_ADDR_2 = 0x68; //9265

// MPU6050
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H1 0x3B
#define ACCEL_XOUT_L1 0x3C
#define GYRO_XOUT_H1 0x43
#define GYRO_XOUT_L1 0x44

// MPU9265
#define PWR_MGMT_2 0x6B
#define ACCEL_XOUT_H2 0x3B
#define ACCEL_XOUT_L2 0x3C
#define GYRO_XOUT_H2 0x43
#define GYRO_XOUT_L2 0x44

int16_t X_out1;
int16_t X_out2;

int counter = 0;
int sum1 = 0;
int sum2 = 0;

// Set these correction factors to 0 when calculating correction factors
const int16_t CF_AX1 = -480;
const int16_t CF_AX2 = 8910;

const int16_t CF_GX1 = -440;
const int16_t CF_GX2 = 1172;


unsigned long lastTime = 0;
float angle = 0;
float alpha = 0.40;
float beta = 0.98;

// @CodeScene(disable:"Excess Number of Function Arguments")
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void calcCorrectionFactor(int16_t val1, int16_t val2) {
	sum1 += val1;
    sum2 += val2;
    counter++; 

    if (counter >= 100) {
        float avg1 = sum1 / 100.0f;
        float avg2 = sum2 / 100.0f;
        Serial.printf("Final Avg 1 = %.2f \t Final Avg 2 = %.2f \n", avg1, avg2);
        while(true) {
            // Halt execution after 100 samples
            delay(10);
        }
    }
}

void displayResult(float val1, float val2, float val3) {
	
	// Serial.printf("MPU1: %.5d MPU2: %.5d \n", X_out1, X_out2);
	Serial.print("Min:");
	Serial.print(-90);
	Serial.print(",");
	Serial.print("Max:");
	Serial.print(90);
	Serial.print(",");
	Serial.print("Accel:");
	Serial.println(val1);
	Serial.print(",");
	Serial.print("Gyro:");
	Serial.println(val2);
	Serial.print(",");
	Serial.print("Angle:");
	Serial.println(val3);
}

void mpu_setup() {
	Wire.begin();
	delay(100);
	Wire.setClock(400000);
	Wire.beginTransmission(MPU_ADDR_1);
	Wire.write(PWR_MGMT_1);
	Wire.write(0x00);
	Wire.endTransmission();

	Wire.beginTransmission(MPU_ADDR_2);
	Wire.write(PWR_MGMT_2);
	Wire.write(0x00);
	Wire.endTransmission();

	lastTime = micros();
}

float get_val(int LOC_H, int16_t CF1, int16_t CF2) {
	int16_t raw1;
	int16_t raw2;
	float val;

	Wire.beginTransmission(MPU_ADDR_1);
	Wire.write(LOC_H);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU_ADDR_1, 2);
	if (Wire.available() >= 2) {
		uint8_t x_h = Wire.read();
		uint8_t x_l = Wire.read();
		// out1 = (int16_t)((x_h << 8) | x_l) + KF_AX1;
		raw1 = (int16_t)((x_h << 8) | x_l) + CF1;
	}

	Wire.beginTransmission(MPU_ADDR_2);
	Wire.write(LOC_H);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU_ADDR_2, 2);
	if (Wire.available() >= 2) {
		uint8_t x_h = Wire.read();
		uint8_t x_l = Wire.read();
		// out2 = (int16_t)((x_h << 8) | x_l) + KF_AX2;
		raw2 = (int16_t)((x_h << 8) | x_l) + CF2;

	}
	
	float out1 = floatMap((float)raw1, -16384, 16384, 90, -90);
	float out2 = floatMap((float)raw2, -16384, 16384, 90, -90);
	
	// calcCorrectionFactor(raw1, raw2);

	return (out1+out2) / 2.0f;
}

float get_angle() {
	static float accel = 0;
	static float gyro = 0;

	unsigned long now = micros();
	float dt = (now - lastTime) / 1000000.0f;
	lastTime = now;

	if (dt<=0) return 0.0;

	float accel_raw = get_val(ACCEL_XOUT_H1, CF_AX1, CF_AX2);
	float gyro_raw = get_val(GYRO_XOUT_H1, CF_GX1, CF_GX2);

	accel = accel * (1-alpha) + accel_raw * alpha;
	gyro = gyro * (1-alpha) + gyro_raw * alpha;

	angle = beta * (angle + gyro*dt) + (1-beta) * accel;
    // displayResult(accel, gyro, angle);

	return angle;
}