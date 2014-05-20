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
#define FUEL_PIN 2
#define ENGINE_PIN 3
#define DHT_PIN 4
#define DS18B20_PIN 7
#define DHT_TYPE DHT22
#define DIGITS 4
#define PRECISION 2

/* --- Interrupts --- */
#define FUEL_INT 0
#define ENGINE_INT 1

/* --- Constants --- */
const int SERIAL_BAUD = 9600;
const int SERIAL_WAIT = 1000; // wait 1000ms before engaging the serial connection
const int INTERVAL = 1000; // sample interval
const int BUFFER_SIZE = 128; // buffer length
const int SENSOR_SIZE = 5;

/* --- Functions --- */
float get_box_temp(void);
float get_box_humidity(void);
float get_engine_lph(int start);
float get_engine_rpm(int start);
float get_engine_temp(void);
void count_fuel_pulse(void);
void count_engine_pulse(void);

/* --- Objects --- */
OneWire oneWire(DS18B20_PIN);
DallasTemperature temperature(&oneWire);
DHT dht(DHT_PIN, DHT_TYPE);

/* --- Variables --- */
volatile int LPH_COUNT = 0;
volatile int RPM_COUNT = 0;
volatile int TIME = 0;

/* --- Buffers --- */
char SENSORS[BUFFER_SIZE];
char BOX_TEMP[SENSOR_SIZE];
char BOX_RH[SENSOR_SIZE];
char ENGINE_RPM[SENSOR_SIZE];
char ENGINE_LPH[SENSOR_SIZE];
char ENGINE_TEMP[SENSOR_SIZE];

/* --- Setup --- */
void setup() {
  delay(SERIAL_WAIT);
  pinMode(FUEL_PIN, INPUT);
  pinMode(ENGINE_PIN, INPUT);
  Serial.begin(SERIAL_BAUD);
  attachInterrupt(FUEL_INT, count_fuel, RISING); // Whenever an interrupt is detected, call the respective counter function
  attachInterrupt(ENGINE_INT, count_engine, RISING);
  dht.begin();
  temperature.begin();
}

/* --- Loop --- */
void loop() {
  dtostrf(get_box_temp(), DIGITS, PRECISION, BOX_TEMP); 
  dtostrf(get_box_humidity(), DIGITS, PRECISION, BOX_RH);
  dtostrf(get_engine_rpm(TIME), DIGITS, PRECISION, ENGINE_RPM);
  dtostrf(get_engine_lph(TIME), DIGITS, PRECISION, ENGINE_LPH);
  dtostrf(get_engine_temp(), DIGITS, PRECISION, ENGINE_TEMP);
  TIME = millis();
  sprintf(SENSORS, "{'box_temp':%s,'box_rh':%s,'engine_lph':%s,'engine_rpm':%s,'engine_temp':%s}", BOX_TEMP, BOX_RH, ENGINE_LPH, ENGINE_RPM, ENGINE_TEMP); // Convert to string and send over serial
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
  float val = temperature.getTempCByIndex(0);
  temperature.requestTemperatures();
  if (isnan(val)) {
    return 0;
  }
  else {
    return val;
  }
}

// Get Fuel LPH() --> Returns LPH of fuel
float get_engine_lph(int start) {
  float val = LPH_COUNT / float(millis() - start);
  LPH_COUNT = 0;
  return val;
}

// Get Engine RPM --> Returns RPM of engine
float get_engine_rpm(int start) {
  float val = RPM_COUNT / float(millis() - start);
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
