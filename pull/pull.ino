 /*
  Control subsystem for MR15
  MEGA 2560
  Handles vehicle steering, dynamic ballast and engine
  
  Responsible For:
     - Seat, Hitch and Button Kills
     - Engine
     - Ballast
     - Steering
    
  Control structure:
    1. Initialize serial, killswitches, relays and motors
    2. If killswitch active --> Kill engine.
    3. If killswitches inactive --> receive command.
    4. If received --> execute command.
    5. If not received --> allow steering.
   
*/

/* --- Libraries --- */
#include <DualVNH5019MotorShield.h>

/* --- Motor Shield Pins (UNO ONLY) --- */
#define STEERING_AMPS_PIN A0
#define ACTUATOR_AMPS_PIN A1
#define MOTOR1_A_PIN 2 // INTERRUPT 0
#define MOTOR1_B_PIN 4
#define MOTOR1_ENABLE_PIN 6
#define MOTOR2_A_PIN 7
#define MOTOR2_B_PIN 8
#define MOTOR1_SPEED_PIN 9
#define MOTOR2_SPEED_PIN 10
#define MOTOR2_ENABLE_PIN 12

/* --- Analog Pins --- */
#define STEERING_POT_PIN A2
#define ACTUATOR_POT_PIN A3
#define GUARD_POUT A14
#define GUARD_PIN A15

/* --- Serial Pins -- */
#define RFID_TX_PIN 16
#define RFID_RX_PIN 17

/* --- Interrupt Pins --- */
#define JOYSTICK_POUT 45
#define BALLAST_UP_PIN 49 // INTERRUPT 5
#define BALLAST_DOWN_PIN 47  // INTERRUPT 4
#define STEERING_UP_PIN 51 // INTERRUPT 3
#define STEERING_DOWN_PIN 53 // INTERRUPT 2

/* --- Switch Pins --- */
#define IGNITION_POUT 22
#define IGNITION_PIN 23
#define SEAT_KILL_POUT 24
#define SEAT_KILL_PIN 25
#define HITCH_KILL_POUT 26
#define HITCH_KILL_PIN 27
#define BUTTON_KILL_POUT 28
#define BUTTON_KILL_PIN 29
#define BRAKES_POUT 30
#define BRAKES_PIN 31
#define NEAR_LIMIT_POUT 32
#define NEAR_LIMIT_PIN 33
#define FAR_LIMIT_POUT 34
#define FAR_LIMIT_PIN 35

/* --- Relay Pins --- */
#define UNUSED_RELAY_PIN 50
#define STOP_RELAY_PIN 48
#define STARTER_RELAY_PIN 46
#define REGULATOR_RELAY_PIN 52

/* --- RFID Commands --- */
#define READ 0x02

/* --- Functions --- */
void kill(void);
void standby(void);
void ignition(void);
void steering(void);
void ballast(void);
boolean check_rfid(void);
boolean check_brakes(void);
boolean check_hitch(void);
boolean check_button(void);
boolean check_seat(void);
void count_ballast_up(void);
void count_ballast_down(void);
void count_steering_up(void);
void count_steering_down(void);

/* --- Constants --- */
const int USB_BAUD = 9600;
const int RFID_BAUD = 9600;
const int IGNITION_WAIT = 250;
const int BALLAST_WAIT = 200;
const int STEERING_WAIT = 200;
const int KILL_WAIT = 2000; 
const int CHECK_WAIT = 10;
const int HITCH_WAIT = 50; // check things
const int SEAT_WAIT = 50;
const int BUTTON_WAIT = 50;
const int STANDBY_WAIT = 20;
const int MOTORS_WAIT = 20;
const int BUFFER_SIZE = 512;
const int STEERING_MIN = 1;
const int STEERING_MAX = 4;
const int BALLAST_MIN = -2;
const int BALLAST_MAX = 2;
const int LIGHT_THRESHOLD = 512;
const int ACTUATOR_RIGHT = 39;
const int ACTUATOR_LEFT = 204;
const int STEERING_RIGHT = 0;
const int STEERING_LEFT = 1023;
const int BALLAST_MULTIPLIER = 200;
const int ACTUATOR_SPEED = 400;
const int NOISE = 30;
const int BALLAST_THRESHOLD = 20000;
const int ERROR_COEF = 50;

/* --- Objects --- */
DualVNH5019MotorShield MOTORS;

/* --- Volatiles --- */
volatile boolean SEAT = false;
volatile boolean HITCH = false;
volatile boolean BUTTON = false;
volatile boolean NEAR = false;
volatile boolean FAR = false;
volatile boolean BRAKES = false;
volatile boolean GUARD = false;
volatile boolean IGNITION = false;
volatile boolean RFID = false;
volatile int STEERING_POSITION = 0;
volatile int ACTUATOR_POSITION = 0;
volatile float STEERING_PERCENT = 0;
volatile float ACTUATOR_PERCENT = 0;
volatile int STEERING_SPEED = STEERING_MIN;
volatile int BALLAST_SPEED = 0;
volatile float ERROR = 0;
volatile float INTEGRAL = 0;
volatile int DUTY_CYCLE = 0;
volatile int STATE = 0;

/* --- Character Buffer --- */
char BUFFER[BUFFER_SIZE];

/* --- Setup --- */
void setup() {
  
  // Enable Serials
  Serial.begin(USB_BAUD);
  Serial3.begin(RFID_BAUD); // Pins 14 and 15
  Serial3.write(READ);
  
  // Enable Analog Inputs
  pinMode(STEERING_POT_PIN, INPUT);
  pinMode(ACTUATOR_POT_PIN, INPUT);
  pinMode(GUARD_PIN, INPUT); digitalWrite(GUARD_PIN, HIGH);

  // Enable Analog Outputs
  pinMode(GUARD_POUT, OUTPUT); digitalWrite(GUARD_POUT, LOW);

  // Enable Digital Switch Inputs
  pinMode(IGNITION_PIN, INPUT); digitalWrite(IGNITION_PIN, HIGH);
  pinMode(BUTTON_KILL_PIN, INPUT); digitalWrite(BUTTON_KILL_PIN, HIGH);
  pinMode(SEAT_KILL_PIN, INPUT); digitalWrite(SEAT_KILL_PIN, HIGH);
  pinMode(HITCH_KILL_PIN,INPUT); digitalWrite(HITCH_KILL_PIN, HIGH);
  pinMode(NEAR_LIMIT_PIN, INPUT); digitalWrite(NEAR_LIMIT_PIN, HIGH);
  pinMode(FAR_LIMIT_PIN, INPUT); digitalWrite(FAR_LIMIT_PIN, HIGH);
  pinMode(BRAKES_PIN, INPUT); digitalWrite(BRAKES_PIN, HIGH);
  pinMode(GUARD_PIN, INPUT); digitalWrite(GUARD_PIN, HIGH);
  
  // Enable Digital Switch Outputs
  pinMode(IGNITION_POUT, OUTPUT); digitalWrite(IGNITION_POUT, LOW);
  pinMode(BUTTON_KILL_POUT, OUTPUT); digitalWrite(BUTTON_KILL_POUT, LOW);
  pinMode(SEAT_KILL_POUT, OUTPUT); digitalWrite(SEAT_KILL_POUT, LOW);
  pinMode(HITCH_KILL_POUT, OUTPUT); digitalWrite(HITCH_KILL_POUT, LOW);
  pinMode(NEAR_LIMIT_POUT, OUTPUT); digitalWrite(NEAR_LIMIT_POUT, LOW);
  pinMode(FAR_LIMIT_POUT, OUTPUT); digitalWrite(FAR_LIMIT_POUT, LOW);
  pinMode(BRAKES_POUT, OUTPUT); digitalWrite(SEAT_KILL_POUT, LOW);
  pinMode(GUARD_POUT, OUTPUT); digitalWrite(GUARD_POUT, LOW);
  
  // Enable Relays
  pinMode(STOP_RELAY_PIN, OUTPUT); digitalWrite(STOP_RELAY_PIN, HIGH);
  pinMode(REGULATOR_RELAY_PIN, OUTPUT); digitalWrite(REGULATOR_RELAY_PIN, HIGH);
  pinMode(STARTER_RELAY_PIN, OUTPUT); digitalWrite(STARTER_RELAY_PIN, HIGH);
  
  // Enable Interrupts
  pinMode(JOYSTICK_POUT, OUTPUT); digitalWrite(JOYSTICK_POUT, LOW);
  pinMode(BALLAST_UP_PIN, INPUT); digitalWrite(BALLAST_UP_PIN, HIGH);// INTERRUPT 5
  pinMode(BALLAST_DOWN_PIN, INPUT); digitalWrite(BALLAST_DOWN_PIN, HIGH);// INTERRUPT 4
  pinMode(STEERING_UP_PIN, INPUT); digitalWrite(STEERING_UP_PIN, HIGH);// INTERRUPT 3
  pinMode(STEERING_DOWN_PIN, INPUT); digitalWrite(STEERING_DOWN_PIN, HIGH);// INTERRUPT 2
}

/* --- Loop --- */
void loop() {
  int a = millis();
  SEAT = check_seat();
  HITCH = check_hitch();
  BUTTON = check_button();
  BRAKES = check_brakes();
  GUARD = check_guard();
  IGNITION = check_ignition();
  RFID = check_rfid();
  count_ballast_up();
  count_ballast_down();
  count_steering_up();
  count_steering_down();
  
  // If in State 0 (OFF) ...
  if (STATE == 0) {
    standby();
    STATE = 1;
  }
  // If in State 1 (STANDBY)...
  else if (STATE == 1) {
    ignition(); // execute ignition
    STATE = 2;
  }
  else if (STATE == 2) {
    if (SEAT || HITCH || BUTTON) {
      kill(); // kill engine
      STATE = 0;
    }
    else {
      standby();
    }
  }
  steering();
  ballast();
  
  // Print to serial
  sprintf(BUFFER,"{'bal_spd':%d,'str_spd':%d,'str_pos':%d,'act_pos':%d,'seat':%d,'brakes':%d,'guard':%d,'hitch':%d,'button':%d,'ignition':%d,'rfid':%d, 'state':%d}",BALLAST_SPEED,STEERING_SPEED,STEERING_POSITION,ACTUATOR_POSITION,SEAT,BRAKES,GUARD,HITCH,BUTTON,IGNITION,RFID,STATE);
  Serial.println(BUFFER);
}

/* --- Engine State Functions --- */
// Kill() --> Kills vehicle 
void kill(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(STARTER_RELAY_PIN, HIGH);
  digitalWrite(REGULATOR_RELAY_PIN, HIGH);
  delay(KILL_WAIT);
}

// Standby() --> Standby vehicle
void standby(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, LOW);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, HIGH);
  delay(STANDBY_WAIT);
}

// Ignition() --> Ignition sequence
void ignition(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  while (check_ignition()) {
    digitalWrite(STOP_RELAY_PIN, LOW);
    digitalWrite(REGULATOR_RELAY_PIN, LOW);
    digitalWrite(STARTER_RELAY_PIN, LOW);
    delay(IGNITION_WAIT);
  }
  digitalWrite(STOP_RELAY_PIN, LOW);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, HIGH);
  delay(STANDBY_WAIT);
}

/* --- Control Functions --- */
// Steering() --> Moves steering actuator
void steering(void) {
  
  // Adjust Steering speed
  if (STEERING_SPEED > STEERING_MAX) {
    STEERING_SPEED = STEERING_MAX;
  }
  else if (STEERING_SPEED < STEERING_MIN) {
    STEERING_SPEED = STEERING_MIN;
  }
  
  // Read Current positions of Actuator and Steering wheel
  STEERING_POSITION = analogRead(STEERING_POT_PIN);
  ACTUATOR_POSITION = analogRead(ACTUATOR_POT_PIN);
  STEERING_PERCENT = 100.0 * STEERING_SPEED * (float(STEERING_POSITION - STEERING_RIGHT) / float(STEERING_LEFT - STEERING_RIGHT));
  if (STEERING_PERCENT > 100) { 
    STEERING_PERCENT = 100;
  }
  ACTUATOR_PERCENT = 100.0 * (float(ACTUATOR_POSITION - ACTUATOR_RIGHT) / float(ACTUATOR_LEFT - ACTUATOR_RIGHT));
  // Serial.println(ACTUATOR_PERCENT);
  // Serial.println(STEERING_PERCENT);
  
  // Calculate PID
  ERROR = STEERING_PERCENT - ACTUATOR_PERCENT;
  DUTY_CYCLE = ERROR_COEF * ERROR;
  if (DUTY_CYCLE > 400) {
    DUTY_CYCLE = 400;
  }
  else if (DUTY_CYCLE < -400) {
    DUTY_CYCLE = -400;
  }  
  MOTORS.setM1Speed(DUTY_CYCLE);
}

// Ballast() --> Moves ballast
void ballast(void) {
  
  // Adjust ballast speed
  if (BALLAST_SPEED > BALLAST_MAX) {
    BALLAST_SPEED = BALLAST_MAX;
  }
  else if (BALLAST_SPEED < BALLAST_MIN) {
    BALLAST_SPEED = BALLAST_MIN;
  }
  
  // If the ballast speed is POSITIVE, activates the ballast FORWARD (POSITIVE) if the far limit is not engaged
  // If the ballast speed is NEGATIVE, activates the ballast BACKWARD (NEGATIVE) if the near limit is not engaged
  if (BALLAST_SPEED > 0) {
    if (MOTORS.getM2CurrentMilliamps() > BALLAST_THRESHOLD) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED * BALLAST_MULTIPLIER);
    }
  }
  else if (BALLAST_SPEED < 0) {
    if (MOTORS.getM2CurrentMilliamps() > BALLAST_THRESHOLD) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED * BALLAST_MULTIPLIER);
    }
  }
  else {
    MOTORS.setM2Speed(0);
  }
  //Serial.println(MOTORS.getM2CurrentMilliamps());
}

/* --- Check Functions --- */
// Check RFID() --> Returns true if RFID detected
boolean check_rfid(void) {
  Serial3.write(READ);
  if (Serial3.read() >= 0) {
    return true;
  }
  else {
    return false;
  }
}

// Check Ignition() --> Returns true if ignition engaged
boolean check_ignition(void) {
  if (!digitalRead(IGNITION_PIN)) {
    delay(CHECK_WAIT);
    if (!digitalRead(IGNITION_PIN)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Check Seat() --> Returns true if seat kill engaged
boolean check_seat(void) {
  if (digitalRead(SEAT_KILL_PIN)) {
    delay(SEAT_WAIT);
    if (digitalRead(SEAT_KILL_PIN)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Check Hitch() --> Returns true if hitch kill engaged
boolean check_hitch(void) {
  if (digitalRead(HITCH_KILL_PIN)) {
    delay(HITCH_WAIT);
    if (digitalRead(HITCH_KILL_PIN)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Check Button() --> Returns true if button kill engaged
boolean check_button(void) {
  if (digitalRead(BUTTON_KILL_PIN)) {
    delay(BUTTON_WAIT);
    if (digitalRead(BUTTON_KILL_PIN)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Check Brakes() --> Returns true if brakes engaged
boolean check_brakes(void) {
  if (digitalRead(BRAKES_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(BRAKES_PIN)) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

// Check Guard() --> Returns true if guard open
boolean check_guard(void) {
  //Serial.println(analogRead(GUARD_PIN));
  if (analogRead(GUARD_PIN) <= LIGHT_THRESHOLD) {
    delay(CHECK_WAIT);
    if (analogRead(GUARD_PIN) <= LIGHT_THRESHOLD) {
      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}


// Check Ballast Up() --> Increases/decreases the ballast speed
void count_ballast_up(void) {
  //Serial.println(digitalRead(BALLAST_UP_PIN));
  if (digitalRead(BALLAST_UP_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(BALLAST_UP_PIN)) {
      BALLAST_SPEED++;
    }
  }
}

// Check Ballast Down() --> Increases/decreases the ballast speed
void count_ballast_down(void) {
  //Serial.println(digitalRead(BALLAST_DOWN_PIN));
  if (digitalRead(BALLAST_DOWN_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(BALLAST_DOWN_PIN)) {
      BALLAST_SPEED--;
    }
  }
}

// Check Steering Up() --> Increases/decreases the steering sensitivity
void count_steering_up(void) {
  //Serial.println(digitalRead(STEERING_UP_PIN));
  if (digitalRead(STEERING_UP_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(STEERING_UP_PIN)) {
      STEERING_SPEED++;
    }
  }
}

// Check Steering Down() --> Increases/decreases the steering sensitivity
void count_steering_down(void) {
  //Serial.println(digitalRead(STEERING_DOWN_PIN));
  if (digitalRead(STEERING_DOWN_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(STEERING_DOWN_PIN)) {
      STEERING_SPEED--;
    }
  }
}
