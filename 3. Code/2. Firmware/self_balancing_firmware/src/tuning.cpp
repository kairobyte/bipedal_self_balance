#include "tuning.h"
#include "servo.h"
#include <Preferences.h>

volatile float kp = 190.0f, ki = 0.0f, kd = 0.6f, kp_boost = 30.0f;
volatile float SET_POINT = -1.0f, DEADBAND = 0.3f;
volatile float acelk = 0.05f, gyrok = 0.05f, beta = 0.995f;
volatile float MAX_COMMAND = 2000.0f, MIN_SPEED = 90.0f;
volatile float servo1 = 52.0f, servo2 = 45.0f, servo3 = 42.0f, servo4 = 130.0f;
volatile float LEAN_GAIN = 0.0f, LEAN_LIMIT = 20.0f;
volatile float CONTROL_INTERVAL_US = 800;

Preferences prefs;

const char* PARAM_LIST[] = {
	"control", "kp","ki","kd", "kp_boost", "SET_POINT","DEADBAND","acelk","gyrok","beta","MAX_COMMAND","MIN_SPEED",
	"servo1","servo2","servo3","servo4","LEAN_GAIN","LEAN_LIMIT"
};
volatile float* PARAM_PTRS[] = {
	&CONTROL_INTERVAL_US,&kp,&ki,&kd, &kp_boost, &SET_POINT,&DEADBAND,&acelk,&gyrok,&beta,&MAX_COMMAND,&MIN_SPEED,
	&servo1,&servo2,&servo3,&servo4,&LEAN_GAIN,&LEAN_LIMIT
};
const int NUM_PARAMS = 18;

void printAll() {
	Serial.println("--- Current parameters ---");
	for (int i = 0; i < NUM_PARAMS; i++) {
		Serial.print(PARAM_LIST[i]);
		Serial.print(" = ");
		Serial.println(*PARAM_PTRS[i], 4);
	}
	Serial.println("Usage: <name>=<value>  |  'save'  |  'print'");
}

void loadFromFlash() {
	prefs.begin("tuning", false);
	for (int i = 0; i < NUM_PARAMS; i++) {
		*PARAM_PTRS[i] = prefs.getFloat(PARAM_LIST[i], *PARAM_PTRS[i]);
	}
	prefs.end();
}

void saveToFlash() {
	prefs.begin("tuning", false);
	for (int i = 0; i < NUM_PARAMS; i++) {
		prefs.putFloat(PARAM_LIST[i], *PARAM_PTRS[i]);
	}
	prefs.end();
	Serial.println("Saved to flash.");
}

bool isServoParam(const String &name) {
	return name == "servo1" || name == "servo2" || name == "servo3" || name == "servo4";
}

void tuning_setup() {
	loadFromFlash();
	servo_move(servo1, servo2, servo3, servo4); // apply loaded/default angles on boot
	printAll();
}

// @CodeScene(disable:"Complex Method")
void tuning_poll() {
	if (!Serial.available()) return;

	String line = Serial.readStringUntil('\n');
	line.trim();
	if (line.length() == 0) return;

	if (line == "print") { printAll(); return; }
	if (line == "save")  { saveToFlash(); return; }

	int eq = line.indexOf('=');
	if (eq == -1) {
		Serial.println("Unrecognized command. Use <name>=<value>, 'print', or 'save'.");
		return;
	}

	String name = line.substring(0, eq);
	float value = line.substring(eq + 1).toFloat();

	bool found = false;
	for (int i = 0; i < NUM_PARAMS; i++) {
		if (name == PARAM_LIST[i]) {
			*PARAM_PTRS[i] = value;
			Serial.print(name);
			Serial.print(" set to ");
			Serial.println(value, 4);
			found = true;
			break;
		}
	}

	if (!found) {
		Serial.print("Unknown parameter: ");
		Serial.println(name);
		return;
	}

	if (isServoParam(name)) {
		servo_move(servo1, servo2, servo3, servo4);
	}
}