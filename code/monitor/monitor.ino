/*
  monitor.ino
  Reads sensors and relays output over serial to Pi
*/

#define KILL_SEAT_PIN 2
#define KILL_HITCH_PIN 3
#define KILL_BUTTON_PIN 4
#define LOCK_GUARD_PIN 5
#define LOCK_BRAKES_PIN 6
#define IGNITION_PIN 7

/* Global Values */
static int BAUD = 9600;
char SENSORS[128];
int KILL = 0;
int LOCK = 0;
int IGNITION = 0;

void setup() {
  pinMode(KILL_SEAT_PIN, INPUT);
  pinMode(KILL_HITCH_PIN, INPUT);
  pinMode(KILL_BUTTON_PIN, INPUT);
  pinMode(LOCK_GUARD_PIN, INPUT);
  pinMode(LOCK_BRAKES_PIN, INPUT);
  pinMode(IGNITION_PIN, INPUT);
  Serial.begin(BAUD);
}

/* Loop */
void loop() {
  KILL = kill();
  LOCK = lock();
  IGNITION = ignition();
  sprintf(SENSORS, "{'kill':%d,'lock':%d,'ignition':%d}",KILL,LOCK,IGNITION); // concatenate message string
  Serial.println(SENSORS);
}

/* Check Killswitches */
int kill(void) {
  if (digitalRead(KILL_SEAT_PIN) || digitalRead(KILL_HITCH_PIN) || digitalRead(KILL_BUTTON_PIN)) {
    return 1;
  }
  else {
    return 0;
  }
}

/* Check Locks */
int lock(void) {
  if (digitalRead(LOCK_GUARD_PIN) || digitalRead(LOCK_BRAKES_PIN)) {
    return 1;
  }
  else {
    return 0;
  }
}

/* Check Ignition */
int ignition(void) {
  if (digitalRead(IGNITION_PIN)) {
    return 1;
  }
  else {
    return 0;
  }
}
