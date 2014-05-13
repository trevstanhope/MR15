/*
  Monitor Subsystem for MR15
  UNO or ALAMODE
  Reads sensors and relays output over serial
*/

/* --- Libraries --- */
#include "DallasTemperature.h" // needed for DS18B20
#include "OneWire.h" // needed for DS18B20
#include "stdio.h" // needed for dtostrf()
#include "DHT.h" // needed for box conditions

/* --- Pins --- */
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define FUEL_PIN 5
#define WHEEL_PIN 6
#define DS18B20_PIN 7

/* --- Interrupts --- */
#define FUEL_INT 0
#define ENGINE_INT 1
#define WHEEL_INT 3

/* --- Constants --- */
const int SERIAL_BAUD = 9600;
const int SERIAL_WAIT = 1000; // wait 1000ms before engaging the serial connection
const int INTERVAL = 1000; // sample interval
const int BUFFER_SIZE = 128; // buffer length

/* --- Functions --- */
float get_box_temp(void);
float get_box_humidity(void);
float get_engine_lph(void);
float get_engine_rpm(void);
float check_engine_temp(void);
void count_fuel_pulse(void);
void count_engine_pulse(void);

/* --- Objects --- */
OneWire oneWire(DS18B20_PIN);
DallasTemperature temperature(&oneWire);
DHT dht(DHT_PIN, DHT_TYPE);

/* --- Variables --- */
volatile int BOX_TEMP = 0;
volatile int BOX_HUMIDITY = 0;
volatile int ENGINE_RPM = 0;
volatile int ENGINE_LPH = 0;
volatile int ENGINE_TEMP = 0;
volatile int LPH_COUNT = 0;
volatile int RPM_COUNT = 0;
volatile int TIME = 0;

/* --- Buffers --- */
char SENSORS[BUFFER_SIZE];

/* --- Setup --- */
void setup() {
  pinMode(FUEL_PIN, INPUT);
  pinMode(WHEEL_PIN, INPUT);
  delay(SERIAL_WAIT);
  Serial.begin(SERIAL_BAUD);
  attachInterrupt(FUEL_INT, count_fuel, RISING); // Whenever an interrupt is detected, call the respective counter function
  attachInterrupt(ENGINE_INT, count_engine, RISING);
  dht.begin();
  temperature.begin();
}

/* --- Loop --- */
void loop() {
  BOX_TEMP = get_box_temp();
  BOX_HUMIDITY = get_box_humidity();
  ENGINE_TEMP = get_engine_temp();
  ENGINE_RPM = get_engine_rpm(TIME);
  ENGINE_LPH = get_engine_lph(TIME);
  TIME = millis();
  sprintf(SENSORS, "{'box_temp':%d,'box_humidity':%d,'engine_lph':%d,'engine_rpm':%d}", BOX_TEMP, BOX_HUMIDITY, ENGINE_LPH, ENGINE_RPM); // Convert to string and send over serial
  Serial.println(SENSORS);
  delay(INTERVAL);
}

/* --- Check Functions --- */
// Get Temp() --> Returns the temperature inside the box
float get_box_temp(void) {
  float val = dht.readTemperature();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

// Get Humidity --> Returns the humidity inside the box
float get_box_humidity(void) {
  float val;
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

// Get Engine Temperature --> Returns engine temperature
float get_engine_temp(void) {
  float val = 0;
  temperature.requestTemperatures();
  val = temperature.getTempCByIndex(0);
  return val;
}

// Get Fuel LPH() --> Returns LPH of fuel
float get_engine_lph(int start) {
  float val = LPH_COUNT / (millis() - start);
  LPH_COUNT = 0;
  return val;
}

// Get Engine RPM --> Returns RPM of engine
float get_engine_rpm(int start) {
  float val = RPM_COUNT / (millis() - start);
  RPM_COUNT = 0;
  return val;
}

/* --- Interrupt Functions --- */
// Count Fuel --> Increments fuel LPH counter
void count_fuel() {
  LPH_COUNT++;
}

// Count Engine --> Incremenets engine RPM counter
void count_engine() {
  RPM_COUNT++;
}
