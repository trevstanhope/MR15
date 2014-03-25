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
     - 
*/

/* --- Libraries --- */
#include <DualVNH5019MotorShield.h>

/* --- Common Pins --- */
#define STEERING_AMPS_PIN A0
#define ACTUATOR_AMPS_PIN A1
#define STEERING_POT_PIN A2
#define ACTUATOR_POT_PIN A3
#define SEAT_KILL_PIN A4
#define HITCH_KILL_PIN A5
#define RX_PIN 0
#define TX_PIN 1
#define MOTOR1_A_PIN 2
#define BUTTON_KILL_PIN 3
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
#define BALLAST_INT 1
#define STEERING_INT 2

/* --- RFID Commans --- */
#define READ 0x02

/* --- Functions --- */
boolean kill(void);
boolean standby(void);
boolean ignition(void);
boolean steering(void);
boolean ballast(void);
void count_ballast(void);
void count_steering(void);

/* --- Time Constants --- */
const int USB_BAUD = 9600;
const int RFID_BAUD = 9600;
const int SERIAL_TIMEOUT = 100;
const int IGNITION_WAIT = 200;
const int BALLAST_WAIT = 200;
const int STEERING_WAIT = 200;
const int KILL_WAIT = 500;
const int CHECK_WAIT = 100;
const int STANDBY_WAIT = 20;
const int MOTORS_WAIT = 20;

/* --- Speed Ranges --- */
const int STEERING_MIN = 0;
const int STEERING_MAX = 100;
const int BALLAST_MIN = 0;
const int BALLAST_MAX = 100;

/* --- Char Commands --- */
const char KILL = 'A';
const char STANDBY = 'B';
const char IGNITION = 'C';
const char STEERING_UP = 'D';
const char STEERING_DOWN = 'E';
const char BALLAST_UP = 'F';
const char BALLAST_DOWN = 'G';
const char NONE = 'H';

/* --- Objects --- */
DualVNH5019MotorShield MOTORS;

/* --- Volatile Bools --- */
volatile boolean killed = false;
volatile boolean locked = false;
volatile boolean limited = false;
volatile boolean kill_seat = false;
volatile boolean kill_hitch = false;
volatile boolean kill_button = false;
volatile boolean limit_near = false;
volatile boolean limit_far = false;
volatile boolean lock_brakes = false;
volatile boolean lock_guard = false;
volatile boolean push_ignition = false;
volatile boolean rfid = false;
volatile boolean ballast_down = false;
volatile boolean ballast_up = false;
volatile boolean steering_down = false;
volatile boolean steering_up = false;

/* --- Volatile Ints --- */
volatile int STEERING_POSITION = 0;
volatile int ACTUATOR_POSITION = 0;
volatile int STEERING_SPEED = 50;
volatile int BALLAST_SPEED = 50;

/* --- Character Buffer --- */
char COMMAND;

/* --- Setup --- */
void setup() {
  Serial1.begin(USB_BAUD);
  Serial2.begin(RFID_BAUD);
  Serial1.setTimeout(SERIAL_TIMEOUT);
  pinMode(BUTTON_KILL_PIN, INPUT);
  pinMode(SEAT_KILL_PIN, INPUT);
  pinMode(HITCH_KILL_PIN,INPUT);
  digitalWrite(BUTTON_KILL_PIN, HIGH);
  digitalWrite(SEAT_KILL_PIN, HIGH);
  digitalWrite(HITCH_KILL_PIN, HIGH);
  
  /* --- Interrupts --- */
  attachInterrupt(BALLAST_INT, count_ballast, CHANGE);
  attachInterrupt(STEERING_INT, count_steering, CHANGE);
}

/* --- Loop --- */
void loop() {
  
  // Check kill and limit and lock twice
  // Then try to get command, if serial is not available default to secondary actions
  kill_seat = digitalRead(SEAT_KILL_PIN);
  kill_hitch = digitalRead(HITCH_KILL_PIN);
  kill_button = digitalRead(BUTTON_KILL_PIN);
  limit_near = digitalRead(NEAR_LIMIT_PIN);
  limit_far = digitalRead(FAR_LIMIT_PIN);
  lock_brakes = digitalRead(BRAKES_PIN);
  lock_guard = digitalRead(GUARD_PIN);
  push_ignition = digitalRead(IGNITION_PIN);
  rfid = check_rfid;
  delay(CHECK_WAIT);
  if (kill_seat && digitalRead(SEAT_KILL_PIN)) {
    kill();
  }
  else if (kill_hitch && digitalRead(HITCH_KILL_PIN)) {
    kill();
    steering();
  }
  else if (kill_button && digitalRead(BUTTON_KILL_PIN)) {
    kill();
  }
  else if (rfid) {
    ready();
    steering();
  }
  else if (push_ignition && (lock_brakes && lock_guard)) {
    ignition();
  }
  else if (limit_near && limit_far) {
    steering();
  }
  else {
    steering();
    ballast();
  }
   
  // Transmit Results
  char json[128];
  char command = Serial1.read();
  switch (command) {
    case KILL:
      kill(); break;
    case STANDBY:
      ready(); break;
    case IGNITION:
      ignition(); break;
    case BALLAST_UP:
      BALLAST_SPEED++; break;
    case BALLAST_DOWN:
      BALLAST_SPEED--; break;
    case STEERING_UP:
      STEERING_SPEED++; break;
    case STEERING_DOWN:
      STEERING_SPEED--; break;
    default:
      break;
  }
  delay(20);
  sprintf(json, "{'ballast':%d, 'steering':%d,'killed':%d,'locked':%d, 'limit':%d}", BALLAST_SPEED, STEERING_SPEED, killed, locked, limited);
  Serial1.println(json);
}

/*
  STATE FUNCTIONS
*/

/* --- Check RFID --- */
boolean check_rfid(void) {
  if (Serial2.available()) {
    return true;
  }
  else {
    return false;
  }
}

/* --- Kill --- */
boolean kill(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, LOW);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  delay(KILL_WAIT);
  return true;
}

/* --- Ready --- */
boolean ready(void) {
  MOTORS.setM1Speed(0);
  MOTORS.setM2Speed(0);
  delay(MOTORS_WAIT);
  digitalWrite(STOP_RELAY_PIN, HIGH);
  digitalWrite(REGULATOR_RELAY_PIN, LOW);
  digitalWrite(STARTER_RELAY_PIN, LOW);
  delay(STANDBY_WAIT);
  return true;
}

/* --- Ignition --- */
boolean ignition(void) {
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
  return true;
}

/* --- Steering --- */
// The right to steer
boolean steering(void) {
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
  return true;
}

/* --- Ballast Speed --- */
boolean ballast(void) {
  MOTORS.setM2Speed(BALLAST_SPEED);
  delay(BALLAST_WAIT);
  MOTORS.setM2Speed(0);
  return true;
}
/*
  CHECKING FUNCTION
*/

/*
  INTERRUPT FUNCTIONS
*/
void count_ballast(void) {
  ballast_down = digitalRead(BALLAST_DOWN_PIN);
  ballast_up = digitalRead(BALLAST_DOWN_PIN);
  if (ballast_down) {
    if (BALLAST_SPEED < BALLAST_MIN) {
      BALLAST_SPEED = BALLAST_MIN;
    }
    else {
      BALLAST_SPEED--;
    }
  }
  else if (ballast_up) {
    if (BALLAST_SPEED > BALLAST_MAX) {
      BALLAST_SPEED = BALLAST_MAX;
    }
    else {
      BALLAST_SPEED++;
    }
  }
}

void count_steering(void) {
  steering_down = digitalRead(BALLAST_DOWN_PIN);
  steering_up = digitalRead(BALLAST_DOWN_PIN);
  if (steering_down) {
    if (STEERING_SPEED < STEERING_MIN) {
      STEERING_SPEED = STEERING_MIN;
    }
    else {
      STEERING_SPEED--;
    }
  }
  else if (steering_up) {
    if (STEERING_SPEED > STEERING_MAX) {
      STEERING_SPEED = STEERING_MAX;
    }
    else {
      STEERING_SPEED++;
    }
  }
}
  
