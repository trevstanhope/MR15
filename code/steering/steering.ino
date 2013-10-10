/*
  steering.ino
  MR15 Steering Sub-System Controller
*/

/* Declarations */
#include <DualVNH5019MotorShield.h>
#define STEERING_POSITION_PIN 1
#define STEERING_A_PIN 2
#define STEERING_B_PIN 3
#define STEERING_INTERRUPT 1

/* Global Values */
DualVNH5019MotorShield ACTUATOR;
volatile int STEERING_POSITION = 0;
volatile int ACTUATOR_POSITION = 0;
static int SENSITIVITY = 100;
static int SPEED = 100;
static int OFF = 0;
static int INTERVAL = 100;

/* Initialize Subsystem */
void setup() {
  pinMode(STEERING_A_PIN, INPUT);
  pinMode(STEERING_B_PIN, INPUT);
  digitalWrite(STEERING_A_PIN, HIGH);
  digitalWrite(STEERING_B_PIN, HIGH);
  attachInterrupt(STEERING_INTERRUPT, encoder, CHANGE);
  ACTUATOR.setM1Speed(0);
}

/* Loop */
void loop() {
  
  if (ACTUATOR_POSITION > (STEERING_POSITION)) {
    while (ACTUATOR_POSITION > (STEERING_POSITION)) {
      ACTUATOR.setM1Speed(SPEED);
      delay(INTERVAL);
      ACTUATOR_POSITION--;
    }
  }
  else if (ACTUATOR_POSITION < (STEERING_POSITION)) {
    while (ACTUATOR_POSITION < (STEERING_POSITION)) {
      ACTUATOR.setM1Speed(-SPEED);
      delay(INTERVAL);
      ACTUATOR_POSITION++;
    }
  }
  else {
    ACTUATOR.setM1Speed(OFF);
  }
}

/* --- Steering Encoder --- */
// Interrupt function to increment the encoder position.
void encoder() {
  if (digitalRead(STEERING_A_PIN) == digitalRead(STEERING_B_PIN)) {
    STEERING_POSITION++;
  }
  else {
    STEERING_POSITION--;
  }
}
