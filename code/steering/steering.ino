#include <DualVNH5019MotorShield.h>
#define STEERING_POSITION_PIN 0
#define ACTUATOR_POSITION_PIN 1

DualVNH5019MotorShield ACTUATOR;
volatile int STEERING_POSITION = 0;
volatile int ACTUATOR_POSITION = 0;

void setup() {
  ACTUATOR.setM1Speed(0);
}

void loop() {
  STEERING_POSITION = analogRead(STEERING_POSITION_PIN);
  ACTUATOR_POSITION = analogRead(ACTUATOR_POSITION_PIN); // 0 - 1024
   
  if (ACTUATOR_POSITION > STEERING_POSITION) {
    ACTUATOR.setM1Speed(100)
  }
  else if (ACTUATOR_POSITION < STEERING_POSITION) {
    ACTUATOR.setM1Speed(-100)
  }
  else {
    ACTUATOR.setM1Speed(0);
  }
}
