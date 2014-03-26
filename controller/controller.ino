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
   
   TODO:
     - Add Ballast limits
     - Add RFID
     - Add Ballast Control
*/

/* --- Libraries --- */
#include <DualVNH5019MotorShield.h>

/* --- Pins --- */
#define STEERING_AMPS_PIN A0
#define ACTUATOR_AMPS_PIN A1
#define STEERING_POT_PIN A2
#define ACTUATOR_POT_PIN A3
#define SEAT_KILL_PIN A4
#define HITCH_KILL_PIN A5
#define RX_PIN 0
#define TX_PIN 1
#define MOTOR1_A_PIN 2 // INTERRUPT 0
#define BUTTON_KILL_PIN 3 // INTERRUPT 1
#define MOTOR1_B_PIN 4
#define STOP_RELAY_PIN 5
#define MOTOR1_ENABLE_PIN 6
#define MOTOR2_A_PIN 7
#define MOTOR2_B_PIN 8
#define MOTOR1_SPEED_PIN 9
#define MOTOR2_SPEED_PIN 10
#define REGULATOR_RELAY_PIN 11
#define MOTOR2_ENABLE_PIN 12
#define STARTER_RELAY_PIN 13
#define BALLAST_PIN 18 // INTERRUPT 5
#define STEERING_PIN 19 // INTERRUPT 4
#define D20 20 // INTERRUPT 3
#define D21 21 // INTERRUPT 2

/* --- Mega Pins --- */
#define RFID_PIN 22
#define NEAR_LIMIT_PIN 23
#define FAR_LIMIT_PIN 24
#define BRAKES_PIN 25
#define GUARD_PIN 26
#define IGNITION_PIN 27
#define BALLAST_UP_PIN 28
#define BALLAST_DOWN_PIN 29

/* --- Interrupts --- */
#define BUTTON_INT 1
#define BALLAST_INT 4
#define STEERING_INT 5

/* --- RFID Commands --- */
#define READ 0x02

/* --- Functions --- */
void kill(void);
void standby(void);
void ignition(void);
void steering(void);
void ballast(void);
void count_ballast(void);
void count_steering(void);
boolean check_rfid(void);

/* --- Constants --- */
const int USB_BAUD = 9600;
const int RFID_BAUD = 9600;
const int SERIAL_TIMEOUT = 100;
const int IGNITION_WAIT = 200;
const int BALLAST_WAIT = 200;
const int STEERING_WAIT = 200;
const int KILL_WAIT = 500;
const int CHECK_WAIT = 10;
const int STANDBY_WAIT = 20;
const int MOTORS_WAIT = 20;
const int SIZE = 256;
const int STEERING_MIN = 0;
const int STEERING_MAX = 100;
const int BALLAST_MIN = 0;
const int BALLAST_MAX = 100;

/* --- Objects --- */
DualVNH5019MotorShield MOTORS;
int STATE = 0;

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
volatile int STEERING_SPEED = 50;
volatile int BALLAST_SPEED = 50;

/* --- Character Buffer --- */
char BUFFER[SIZE];

/* --- Setup --- */
void setup() {
  Serial.begin(USB_BAUD);
  Serial2.begin(RFID_BAUD);
  Serial.setTimeout(SERIAL_TIMEOUT);
  pinMode(BUTTON_KILL_PIN, INPUT);
  pinMode(SEAT_KILL_PIN, INPUT);
  pinMode(HITCH_KILL_PIN,INPUT);
  pinMode(NEAR_LIMIT_PIN, INPUT);
  pinMode(FAR_LIMIT_PIN, INPUT);
  pinMode(BRAKES_PIN, INPUT);
  pinMode(GUARD_PIN, INPUT);
  digitalWrite(BUTTON_KILL_PIN, LOW);
  digitalWrite(SEAT_KILL_PIN, LOW);
  digitalWrite(HITCH_KILL_PIN, LOW);
  digitalWrite(NEAR_LIMIT_PIN, LOW);
  digitalWrite(FAR_LIMIT_PIN, LOW);
  digitalWrite(BRAKES_PIN, LOW);
  digitalWrite(GUARD_PIN, LOW);
  attachInterrupt(BALLAST_INT, count_ballast, CHANGE);
  attachInterrupt(STEERING_INT, count_steering, CHANGE);
}

/* --- Loop --- */
void loop() {
  SEAT = check_seat();
  HITCH = check_hitch();
  BUTTON = check_button();
  BRAKES = check_brakes();
  GUARD = check_guard();
  NEAR = check_near();
  FAR = check_far();
  IGNITION = check_ignition();
  RFID = check_rfid();
  if (STATE == 0) {
    if (RFID) {
      ready();
      STATE = 1;
    }
  }
  else if (STATE == 1) {
    if (SEAT || HITCH || BUTTON) {
      kill();
      STATE = 1;
    }
    else if (IGNITION && !BRAKES && !GUARD) {
      ignition();
      STATE = 2;
    }
    else {
      steering();
      ballast();
    }
  }
  else if (STATE == 2) {
    if (SEAT || HITCH || BUTTON) {
      kill();
      STATE = 1;
    }
    else {
      steering();
      ballast();
    }
  }
  else {
    STATE = 0;
  }
  sprintf(BUFFER, "{'ballast':%d,'steering':%d,'seat':%d,'brakes':%d,'guard':%d,'hitch':%d,'button':%d,'near':%d,'far':%d,'ignition':%d,'state':%d}", BALLAST_SPEED,STEERING_SPEED,SEAT,BRAKES,GUARD,HITCH,BUTTON,NEAR,FAR,IGNITION,STATE);
  Serial.println(BUFFER);
}

/* --- State Functions --- */
// Kill() --> Kills vehicle then returns true 
void kill(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, LOW);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  delay(KILL_WAIT);
}

// Ready() --> Ready vehicle
void ready(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, LOW);
  delay(STANDBY_WAIT);
}

// Ignition() --> Ignition
void ignition(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, HIGH);
  delay(IGNITION_WAIT);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, LOW);
  delay(STANDBY_WAIT);
}

// Steering() --> Moves actuator
void steering(void) {
  STEERING_POSITION = analogRead(STEERING_POT_PIN);
  ACTUATOR_POSITION = analogRead(ACTUATOR_POT_PIN);
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

// Ballast() --> Moves ballast
void ballast(void) {
  if (BALLAST_SPEED > 0) {
    if (check_far()) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED);
      delay(BALLAST_WAIT);
      MOTORS.setM2Speed(0);
    }
  }
  else if (BALLAST_SPEED < 0) {
    if (check_near()) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED);
      delay(BALLAST_WAIT);
      MOTORS.setM2Speed(0);
    }
  }
  else {
    MOTORS.setM2Speed(0);
  }
}

/* --- CHECKING FUNCTION --- */
// Check RFID() --> Returns true if RFID valid
boolean check_rfid(void) {
  if (Serial2.available()) {
    return true;
  }
  else {
    return false;
  }
}

// Check Ignition() --> Returns true if ignition pressed
boolean check_ignition(void) {
  if (digitalRead(IGNITION_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(IGNITION_PIN)) {
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
    delay(CHECK_WAIT);
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

// Check Hitch() --> Returns true if seat kill engaged
boolean check_hitch(void) {
  if (digitalRead(HITCH_KILL_PIN)) {
    delay(CHECK_WAIT);
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

// Check Button() --> Returns true if seat kill engaged
boolean check_button(void) {
  if (digitalRead(BUTTON_KILL_PIN)) {
    delay(CHECK_WAIT);
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
  if (digitalRead(BRAKES_PIN) || digitalRead(GUARD_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(BRAKES_PIN) || digitalRead(GUARD_PIN)) {
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
  if (digitalRead(GUARD_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(GUARD_PIN)) {
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

// Check Near() --> Returns true if limits engaged
boolean check_near(void) {
  if (digitalRead(NEAR_LIMIT_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(NEAR_LIMIT_PIN)) {
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

// Check Far() --> Returns true if limits engaged
boolean check_far(void) {
  if (digitalRead(FAR_LIMIT_PIN)) {
    delay(CHECK_WAIT);
    if (digitalRead(FAR_LIMIT_PIN)) {
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

/* --- INTERRUPT FUNCTIONS --- */
// Count Ballast() --> 
void count_ballast(void) {
  if (digitalRead(BALLAST_DOWN_PIN)) {
    if (BALLAST_SPEED < BALLAST_MIN) {
      BALLAST_SPEED = BALLAST_MIN;
    }
    else {
      BALLAST_SPEED--;
    }
  }
  else if (digitalRead(BALLAST_DOWN_PIN)) {
    if (BALLAST_SPEED > BALLAST_MAX) {
      BALLAST_SPEED = BALLAST_MAX;
    }
    else {
      BALLAST_SPEED++;
    }
  }
}

// Count Steering() -->
void count_steering(void) {
  if (digitalRead(BALLAST_DOWN_PIN)) {
    if (STEERING_SPEED < STEERING_MIN) {
      STEERING_SPEED = STEERING_MIN;
    }
    else {
      STEERING_SPEED--;
    }
  }
  else if (digitalRead(BALLAST_DOWN_PIN)) {
    if (STEERING_SPEED > STEERING_MAX) {
      STEERING_SPEED = STEERING_MAX;
    }
    else {
      STEERING_SPEED++;
    }
  }
}
