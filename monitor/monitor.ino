/*
  Monitor Subsystem for MR15
  UNO or ALAMODE
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
      'humidity': float,
      'engine':float
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

/* --- Interrupts --- */
#define FUEL_INT 0
#define WHEEL_INT 1

/* --- Constants --- */
#define SERIAL_BAUD 9600
#define SERIAL_TIMEOUT 10 // disregard transmission after 10 ms 
#define SERIAL_WAIT 1000 // wait 1000ms before engaging the serial connection
#define DHT_TYPE 22
#define INTERVAL 100
#define SIZE 128

/* --- Functions --- */
float check_temp(void);
float check_humidity(void);
float check_fuel(void);
float check_wheel(void);
void count_wheel(void);
void count_fuel(void);

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
char SENSORS[SIZE];

/* --- Setup --- */
void setup() {
  pinMode(FUEL_PIN, INPUT);
  pinMode(WHEEL_PIN, INPUT);
  dht.begin();
  delay(SERIAL_WAIT);
  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(SERIAL_TIMEOUT);
  attachInterrupt(FUEL_INT, count_fuel, RISING); // Whenever an interrupt is detected, call the respective counter function
  attachInterrupt(WHEEL_INT, count_wheel, CHANGE);
}

/* --- Loop --- */
void loop() {
  TEMP = check_temp();
  HUMIDITY = check_humidity();
  FUEL = check_fuel(TIME);
  WHEEL = check_wheel(TIME);
  TIME = millis();
  sprintf(SENSORS, "{'temp':%d,'humidity':%d,'fuel':%d,'wheel':%d}", TEMP, HUMIDITY, FUEL, WHEEL); // Convert to string and send over serial
  Serial.println(SENSORS);
  delay(INTERVAL);
}

/* --- Check Functions --- */
// Check Temp() --> Returns the temperature inside the box
float check_temp() {
  float val = dht.readTemperature();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

// Check Humidity() --> Returns the humidity inside the box
float check_humidity() {
  float val;
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

// Check Wheel() --> Returns RPM of wheel
float check_wheel(int start) {
  float val = WHEEL_COUNT / (millis() - start);
  WHEEL_COUNT = 0;
  return val;
}

// Check Fuel() --> Returns LPH of fuel
float check_fuel(int start) {
  float val = FUEL_COUNT / (millis() - start);
  FUEL_COUNT = 0;
  return val;
}

// Check Engine() --> Returns LPH of fuel
float check_fuel(int start) {
  float val = FUEL_COUNT / (millis() - start);
  FUEL_COUNT = 0;
  return val;
}

/* --- Interrupt Functions --- */
// Count Wheel() --> Increments wheel counter
void count_wheel() {
  WHEEL_COUNT++;
}

// Count Fuel() --> Increments fuel counter
void count_fuel() {
  FUEL_COUNT++;
}

// Count Engine() --> Incremenets engine RPM counter
void count_engine() {
  ENGINE_COUNT++;
}
