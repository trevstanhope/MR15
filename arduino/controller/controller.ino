/*
  CONTROLLER
  Control subsystem for MR15
  Handles vehicle steering, dynamic ballast and engine
*/

/* --- Libraries --- */
#include <DualVNH5019MotorShield.h>

/* --- Pins --- */
#define STEERING_POSITION_PIN 0 // Steering potentiometer
#define ACTUATOR_POSITION_PIN 1 // Actuator potentiometer
#define BALLAST_SPEED_PIN 2 // Ballast potentiometer
#define STOP_RELAY_PIN 2
#define REGULATOR_RELAY_PIN 3
#define STARTER_RELAY_PIN 4

/* --- Commands --- */
const int KILL = 0;
const int STANDBY = 1;
const int IGNITION = 2;
const int RUN = 3;

/* --- Objects --- */
DualVNH5019MotorShield MOTORS;

/* --- Constants --- */
const int BAUD = 115200;

/* --- Variables --- */
volatile int STEERING_POSITION = 0;
volatile int ACTUATOR_POSITION = 0;
volatile int STEERING_SPEED = 100;
char COMMAND;
boolean STOP_RELAY = false;
boolean STARTER_RELAY = false;
boolean REGULATOR_RELAY = false;

/* --- Setup --- */
void setup() {
  Serial.begin(BAUD);
}

/* --- Loop --- */
void loop() {

  // Attempt to get command
  if (Serial.available()) {
    char COMMAND = Serial.read();
    switch (COMMAND) {
      case STOP:
        STOP_RELAY = true;
        STARTER_RELAY = false;
        REGULATOR_RELAY = false;
        break;
      case STANDBY:
        STOP_RELAY = false;
        STARTER_RELAY = false;
        REGULATOR_RELAY = true;
        break;
      case IGNITION:
        STOP_RELAY = false;
        STARTER_RELAY = true;
        REGULATOR_RELAY = false;
        break;
      case RUN:
        STOP_RELAY = false;
        STARTER_RELAY = false;
        REGULATOR_RELAY = true;
        break;
      default:
        break;
    }
  }
 
  // Handle Engine 
  if (STOP_RELAY) {
    digitalWrite(STOP_RELAY_PIN, LOW);
  }
  else {
    digitalWrite(STOP_RELAY_PIN, HIGH);
  }
  if (REGULATOR_RELAY) {
    digitalWrite(REGULATOR_RELAY_PIN, LOW);
  }
  else {
    digitalWrite(REGULATOR_RELAY_PIN, HIGH);
  }
  if (STARTER_RELAY) {
    digitalWrite(STARTER_RELAY_PIN, LOW);
  }
  else {
    digitalWrite(STARTER_RELAY_PIN, HIGH);
  }
  
  // Handle steering
  STEERING_POSITION = analogRead(STEERING_POSITION_PIN);
  ACTUATOR_POSITION = analogRead(ACTUATOR_POSITION_PIN);
  if (ACTUATOR_POSITION > STEERING_POSITION) {
    MOTORS.setM1Speed(STEERING_SPEED);
  }
  else if (ACTUATOR_POSITION < STEERING_POSITION) {
    MOTORS.setM1Speed(-STEERING_SPEED);
  }
  else {
    MOTORS.setM1Speed(0);
  }
  
}
