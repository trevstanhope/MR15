/*
  MONITOR
  subsystem for MR15
  Reads sensors and relays output over serial to Pi
*/

/* --- Libraries --- */
#include <SoftwareSerial.h> // needed for RFID module.
#include "stdio.h" // needed for dtostrf()

/* --- Pins --- */
#define SEAT_PIN 2
#define HITCH_PIN 3
#define BUTTON_PIN 4
#define GUARD_PIN 5
#define BRAKE_PIN 6
#define IGNITION_PIN 7

/* --- Constants --- */
const int BAUD = 115200; 

/* --- Variables --- */
char SENSORS[128];
volatile int SEAT = 0;
volatile int HITCH = 0;
volatile int IGNITION = 0;
volatile int BUTTON = 0;
volatile int GUARD = 0;
volatile int BRAKE = 0;

/* --- Setup --- */
void setup() {

  // Initialize Pins
  pinMode(SEAT_PIN, INPUT);
  pinMode(HITCH_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GUARD_PIN, INPUT);
  pinMode(BRAKE_PIN, INPUT);
  pinMode(IGNITION_PIN, INPUT);

  // Normally One - Enable Resistor
  digitalWrite(SEAT_PIN, HIGH);
  digitalWrite(HITCH_PIN, HIGH);
  digitalWrite(BUTTON_PIN, HIGH);
  digitalWrite(GUARD_PIN, HIGH);
  digitalWrite(BRAKE_PIN, HIGH);
  digitalWrite(IGNITION_PIN, HIGH);

  // Initialize Serial
  delay(1000);
  Serial.begin(BAUD);

}

/* --- Loop --- */
void loop() {
  
  // Read Pins
  SEAT = digitalRead(SEAT_PIN);
  HITCH = digitalRead(HITCH_PIN);
  BUTTON = digitalRead(BUTTON_PIN);
  GUARD = digitalRead(GUARD_PIN);
  BRAKE = digitalRead(BRAKE_PIN);
  IGNITION = digitalRead(IGNITION_PIN);

  // Convert to string
  sprintf(SENSORS, "{'seat':%d,'hitch':%d,'button':%d,'brake':%d,'guard':%d,'ignition':%d}",SEAT,HITCH,BUTTON,BRAKE,GUARD,IGNITION); // concatenate message string

  // Send over serial
  Serial.println(SENSORS);
  Serial.flush();
}

