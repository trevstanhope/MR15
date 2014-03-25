/*
  Monitor Subsystem for MR15
  UNO
  Reads sensors and relays output over serial
  
  Responsible for the following:
    - Fuel Flow Sensor
    - Wheel RPM Sensor
    - Temperature/Humidity Sensor (DHT22)
    
  Output:
    {
      'fuel': float,
      'wheel': float,
      'temp': float,
      'humidity': float
    }
*/

/* --- Libraries --- */
#include <SoftwareSerial.h> // needed for RFID module.
#include "stdio.h" // needed for dtostrf()
#include "DHT.h"

/* --- Pins --- */
#define DHT_PIN 4
#define FUEL_PIN 5
#define WHEEL_PIN 6

/* --- Functions --- */
float check_temp(void);
float check_humidity(void);
float check_fuel(void);
float check_wheel(void);
void count_wheel(void);
void count_fuel(void);

/* --- Interrupts --- */
#define FUEL_INT 0
#define WHEEL_INT 1

/* --- Constants --- */
#define BAUD 9600
#define DHT_TYPE 22
#define INTERVAL 200

/* --- Objects --- */
DHT dht(DHT_PIN, DHT_TYPE);

/* --- Variables --- */
volatile int TEMP = 0;
volatile int HUMIDITY = 0;
volatile int FUEL = 0;
volatile int WHEEL = 0;
volatile int FUEL_COUNT = 0;
volatile int WHEEL_COUNT = 0;
volatile int TIME = 0;

/* --- Buffers --- */
char SENSORS[128];

/* --- Setup --- */
void setup() {

  // Initialize On/Off Pins
  pinMode(FUEL_PIN, INPUT);
  pinMode(WHEEL_PIN, INPUT);
  
  // Initialize DHT
  dht.begin();
  
  // Initialize Serial interfaces
  Serial.begin(BAUD);
  
  // Interrupts
  // Whenever an interrupt is detected, call the respective counter function
  attachInterrupt(FUEL_INT, count_fuel, RISING);
  attachInterrupt(WHEEL_INT, count_wheel, CHANGE);
}

/* --- Loop --- */
void loop() {
  
  // Read Sensors
  TEMP = check_temp();
  HUMIDITY = check_humidity();
  FUEL = check_fuel(TIME);
  WHEEL = check_wheel(TIME);
  TIME = millis();

  // Convert to string and send over serial
  sprintf(SENSORS, "{'temp':%d,'humidity':%d,'fuel':%d,'wheel':%d}", TEMP,HUMIDITY,FUEL,WHEEL);
  Serial.println(SENSORS);
  delay(INTERVAL);
}

/*
  FUNCTIONS
*/

/* --- Check Temp --- */
// Returns the temperature inside the box
float check_temp() {
  float val = dht.readTemperature();
  if (val) {
    return val;
  }
  else {
    return 0;
  }
}

/* --- Check Humidity --- */
// Returns the humidity inside the box
float check_humidity() {
  float val;
  if (val) {
    return val;
  }
  else {
    return 0;
  }
}

/* --- Check Wheel -- */
// Return RPM of wheel
float check_wheel(int start) {
  float val = WHEEL_COUNT / (millis() - start);
  WHEEL_COUNT = 0;
  return val;
}

/* --- Check Fuel -- */
// Returns LPH of fuel
float check_fuel(int start) {
  float val = FUEL_COUNT / (millis() - start);
  FUEL_COUNT = 0;
  return val;
}

/* --- Count Wheel --- */
void count_wheel() {
  WHEEL_COUNT++;
}

/* --- Count Fuel --- */
void count_fuel() {
  FUEL_COUNT++;
}
