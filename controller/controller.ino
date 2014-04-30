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

/* --- Pins --- */
#define STEERING_AMPS_PIN A0
#define ACTUATOR_AMPS_PIN A1
#define STEERING_POT_PIN A2
#define ACTUATOR_POT_PIN A3
//#define SEAT_KILL_PIN A4
//#define HITCH_KILL_PIN A5
#define RX_PIN 0
#define TX_PIN 1
#define MOTOR1_A_PIN 2 // INTERRUPT 0
// #define BUTTON_KILL_PIN 3 // INTERRUPT 1
#define MOTOR1_B_PIN 4
//#define STOP_RELAY_PIN 5
#define MOTOR1_ENABLE_PIN 6
#define MOTOR2_A_PIN 7
#define MOTOR2_B_PIN 8
#define MOTOR1_SPEED_PIN 9
#define MOTOR2_SPEED_PIN 10
//#define REGULATOR_RELAY_PIN 11
#define MOTOR2_ENABLE_PIN 12
//#define STARTER_RELAY_PIN 13

/* --- Serial/Interrupt Pins --- */
#define RFID_TX_PIN 14
#define RFID_RX_PIN 15
#define BALLAST_UP_PIN 18 // INTERRUPT 5
#define BALLAST_DOWN_PIN 19 // INTERRUPT 4
#define STEERING_UP_PIN 20 // INTERRUPT 3
#define STEERING_DOWN_PIN 21 // INTERRUPT 2

/* --- Switch Pins --- */
#define NEAR_LIMIT_PIN 22
#define FAR_LIMIT_PIN 23
#define BRAKES_PIN 24
#define GUARD_PIN 25
#define IGNITION_PIN 26
#define SEAT_KILL_PIN 27
#define HITCH_KILL_PIN 28
#define BUTTON_KILL_PIN 29

/* --- Relay Pins --- */
#define STOP_RELAY_PIN 30
#define STARTER_RELAY_PIN 31
#define REGULATOR_RELAY_PIN 32

/* --- Interrupts --- */
#define BALLAST_UP_INT 5 // PIN 18
#define BALLAST_DOWN_INT 4 // PIN 19
#define STEERING_UP_INT 3 // PIN 20
#define STEERING_DOWN_INT 2 // PIN 21

/* --- RFID Commands --- */
#define READ 0x02

/* --- Functions --- */
void kill(void);
void standby(void);
void ignition(void);
void steering(void);
void ballast(void);
void count_ballast_up(void);
void count_steering_up(void);
void count_ballast_down(void);
void count_steering_down(void);
boolean check_rfid(void);
boolean check_near(void);
boolean check_far(void);
boolean check_brakes(void);
boolean check_hitch(void);
boolean check_button(void);
boolean check_seat(void);

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
const int BUFFER_SIZE = 512;
const int STEERING_MIN = 0;
const int STEERING_NORMAL = 2;
const int STEERING_MAX = 4;
const int STEERING_MULTIPLIER = 25;
const int BALLAST_MIN = -2;
const int BALLAST_MAX = 2;
const int BALLAST_MULTIPLIER = 25;
const int LIGHT_THRESHOLD = 50;

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
volatile int STEERING_SPEED = STEERING_NORMAL;
volatile int BALLAST_SPEED = 0;
volatile int STATE = 0;


/* --- Character Buffer --- */
char BUFFER[BUFFER_SIZE];

/* --- Setup --- */
void setup() {
  Serial.begin(USB_BAUD);
  //Serial3.begin(RFID_BAUD); // Pins 14 and 15
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
  attachInterrupt(BALLAST_UP_INT, count_ballast_up, RISING);
  attachInterrupt(BALLAST_DOWN_INT, count_steering_down, RISING);
  attachInterrupt(STEERING_UP_INT, count_steering_up, RISING);
  attachInterrupt(STEERING_DOWN_INT, count_steering_down, RISING);
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
      standby();
      STATE = 1;
    }
  }
  else if (STATE == 1) {
    if (SEAT || HITCH || BUTTON) {
      kill(); // kill engine
      STATE = 1;
    }
    else if (IGNITION && !BRAKES && !GUARD) {
      ignition(); // execute ignition
      STATE = 2;
    }
    else {
      steering();
      ballast();
    }
  }
  else if (STATE == 2) {
    if (SEAT || HITCH || BUTTON) {
      kill(); // kill engine
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
  sprintf(
    BUFFER,
    "{'bal_spd':%d,'str_spd':%d,'str_pos':%d,'act_pos':%d,'seat':%d,'brakes':%d,'guard':%d,'hitch':%d,'button':%d,'near':%d,'far':%d,'ignition':%d,'state':%d}",
    BALLAST_SPEED,STEERING_SPEED,STEERING_POSITION,ACTUATOR_POSITION,SEAT,BRAKES,GUARD,HITCH,BUTTON,NEAR,FAR,IGNITION,STATE);
  Serial.println(BUFFER);
}

/* --- Engine State Functions --- */
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

// standby() --> standby vehicle
void standby(void) {
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

/* --- Control Functions --- */
// Steering() --> Moves steering actuator
void steering(void) {
  STEERING_POSITION = analogRead(STEERING_POT_PIN);
  ACTUATOR_POSITION = analogRead(ACTUATOR_POT_PIN);
  // If the actuator is returning a position LEFT of the steering wheel, activate the actuator to go RIGHT (NEGATIVE)
  if (ACTUATOR_POSITION < STEERING_POSITION) {
    MOTORS.setM1Speed(-STEERING_SPEED*STEERING_MULTIPLIER);
  }
  // If the actuator is returning a position RIGHT of the steering wheel, activate the actuator to go LEFT (POSITIVE)
  else if (ACTUATOR_POSITION > STEERING_POSITION) {
    MOTORS.setM1Speed(STEERING_SPEED*STEERING_MULTIPLIER);
  }
  else {
    MOTORS.setM1Speed(0);
  }
}

// Ballast() --> Moves ballast
void ballast(void) {
  // If the ballast speed is positive, activates the ballast FORWARD (POSITIVE) if the far limit is not engaged
  if (BALLAST_SPEED > 0) {
    if (check_far()) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED*BALLAST_MULTIPLIER);
      delay(BALLAST_WAIT);
      MOTORS.setM2Speed(0);
    }
  }
  // If the ballast speed is NEGATIVE, activates the ballast BACKWARD (NEGATIVE) if the near limit is not engaged
  else if (BALLAST_SPEED < 0) {
    if (check_near()) {
      MOTORS.setM2Speed(0);
    }
    else {
      MOTORS.setM2Speed(BALLAST_SPEED*BALLAST_MULTIPLIER);
      delay(BALLAST_WAIT);
      MOTORS.setM2Speed(0);
    }
  }
  else {
    MOTORS.setM2Speed(0);
  }
}

/* --- Check Functions --- */
// Check RFID() --> Returns true if RFID valid
boolean check_rfid(void) {
//  if (Serial3.available()) {
//    return true;
//  }
//  else {
//    return false;
//  }
  return true;
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
  if (digitalRead(GUARD_PIN) >= LIGHT_THRESHOLD) {
    delay(CHECK_WAIT);
    if (digitalRead(GUARD_PIN) >= LIGHT_THRESHOLD) {
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

// Check Far() --> Returns true if switch engaged
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

/* --- Interrupt Function --- */
// Count Ballast Up() --> Increases the ballast speed
void count_ballast_up(void) {
  if (BALLAST_SPEED > BALLAST_MAX) {
    BALLAST_SPEED = BALLAST_MAX; // limit at max
  }
  else {
    BALLAST_SPEED++;
  }
}

// Count Ballast Down() --> Decreases the ballast speed
void count_ballast_down(void) {
  if (BALLAST_SPEED < BALLAST_MIN) {
    BALLAST_SPEED = BALLAST_MIN; // limit at min
  }
  else {
    BALLAST_SPEED--;
  }
}

// Count steering Up() --> Increases the steering sensitivity
void count_steering_up(void) {
  if (STEERING_SPEED > STEERING_MAX) {
    STEERING_SPEED = STEERING_MAX; // limit at max
  }
  else {
    STEERING_SPEED++;
  }
}

// Count Steering Down() --> Decreases the steering sensitivity
void count_steering_down(void) {
  if (STEERING_SPEED < STEERING_MIN) {
    STEERING_SPEED = STEERING_MIN; // limit at min
  }
  else {
    STEERING_SPEED--;
  }
}
