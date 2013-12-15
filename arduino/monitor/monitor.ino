/*
  MONITOR
  subsystem for MR15
  Reads sensors and relays output over serial to Pi
*/

/* --- Libraries --- */
/* --- Pins --- */
#define KILL_SEAT_PIN 2
#define KILL_HITCH_PIN 3
#define KILL_BUTTON_PIN 4
#define LOCK_GUARD_PIN 5
#define LOCK_RIGHTBRAKE_PIN 6
#define LOCK_LEFTBRAKE_PIN 7
#define IGNITION_PIN 8

/* --- Constants --- */
const int BAUD = 115200; 

/* --- Variables --- */
char SENSORS[128];
volatile int SEAT = 0;
volatile int HITCH = 0;
volatile int IGNITION = 0;
volatile int BUTTON = 0;
volatile int GUARD = 0;
volatile int LEFTBRAKE = 0;
volatile int RIGHTBRAKE = 0;

/* --- Setup --- */
void setup() {
  Serial.begin(BAUD);
  pinMode(KILL_SEAT_PIN, INPUT);
  pinMode(KILL_HITCH_PIN, INPUT);
  pinMode(KILL_BUTTON_PIN, INPUT);
  pinMode(LOCK_GUARD_PIN, INPUT);
  pinMode(LOCK_LEFTBRAKE_PIN, INPUT);
  pinMode(LOCK_RIGHTBRAKE_PIN, INPUT);
  pinMode(IGNITION_PIN, INPUT);
  digitalWrite(KILL_SEAT_PIN, HIGH);
  digitalWrite(KILL_HITCH_PIN, HIGH);
  digitalWrite(KILL_BUTTON_PIN, HIGH);
  digitalWrite(LOCK_GUARD_PIN, HIGH);
  digitalWrite(LOCK_LEFTBRAKE_PIN, HIGH);
  digitalWrite(LOCK_RIGHTBRAKE_PIN, HIGH);
  digitalWrite(IGNITION_PIN, HIGH);
}

/* --- Loop --- */
void loop() {
  SEAT = digitalRead(KILL_SEAT_PIN);
  HITCH = digitalRead(KILL_HITCH_PIN);
  BUTTON = digitalRead(KILL_BUTTON_PIN);
  GUARD = digitalRead(LOCK_GUARD_PIN);
  LEFTBRAKE = digitalRead(LOCK_LEFTBRAKE_PIN);
  RIGHTBRAKE = digitalRead(LOCK_RIGHTBRAKE_PIN);
  sprintf(SENSORS, "{'seat':%d,'hitch':%d,'button':%d,'left_brake':%d,'right_brake':%d,'guard':%d,'ignition':%d}",SEAT,HITCH,BUTTON,LEFTBRAKE,RIGHTBRAKE,GUARD,IGNITION); // concatenate message string
  Serial.println(SENSORS);
  Serial.flush();
}

