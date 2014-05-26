/*
  Monitor Subsystem for MR15
  UNO or ALAMODE
  Reads sensors and relays output over serial
*/

/* --- Libraries --- */
#include "DallasTemperature.h" // needed for DS18B20
#include "OneWire.h" // needed for DS18B20
#include "stdio.h" // needed for dtostrf()

/* --- Pins --- */
#define FUEL_PIN 2
#define ENGINE_PIN 3
#define DHT_PIN 4
#define DS18B20_PIN 5
#define DIGITS 4
#define PRECISION 2

/* --- Interrupts --- */
#define FUEL_INT 0
#define ENGINE_INT 1

/* --- Constants --- */
const int SERIAL_BAUD = 9600;
const int SERIAL_WAIT = 10000; // wait 10s before engaging the serial connection
const int INTERVAL = 1000; // sample interval
const int BUFFER_SIZE = 128; // buffer length
const int SENSOR_SIZE = 5;

/* --- Functions --- */
float get_engine_lph(void);
float get_engine_rpm(void);
float get_engine_temp(void);
void count_fuel(void);
void count_engine(void);

/* --- Objects --- */
OneWire oneWire(DS18B20_PIN);
DallasTemperature temperature(&oneWire);

/* --- Variables --- */
volatile int LPH_COUNT = 0;
volatile int RPM_COUNT = 0;
int LPH_A = 0;
int LPH_B = 0;
int RPM_A = 0;
int RPM_B = 0;

/* --- Buffers --- */
char SENSORS[BUFFER_SIZE];
char ENGINE_RPM[SENSOR_SIZE];
char ENGINE_LPH[SENSOR_SIZE];
char ENGINE_TEMP[SENSOR_SIZE];

/* --- Setup --- */
void setup() {
  delay(SERIAL_WAIT);
  Serial.begin(SERIAL_BAUD);
  attachInterrupt(FUEL_INT, count_fuel, RISING); // Whenever an interrupt is detected, call the respective counter function
  attachInterrupt(ENGINE_INT, count_engine, RISING);
  temperature.begin();
}

/* --- Loop --- */
void loop() {
  dtostrf(get_engine_rpm(), DIGITS, PRECISION, ENGINE_RPM);
  dtostrf(get_engine_lph(), DIGITS, PRECISION, ENGINE_LPH);
  dtostrf(get_engine_temp(), DIGITS, PRECISION, ENGINE_TEMP);
  sprintf(SENSORS, "{'engine_lph':%s,'engine_rpm':%s,'engine_temp':%s}", ENGINE_LPH, ENGINE_RPM, ENGINE_TEMP); // Convert to string and send over serial
  Serial.println(SENSORS);
  delay(INTERVAL);
}

/* --- Check Functions --- */
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
float get_engine_lph(void) {
  LPH_B = millis();
  float val = (float(LPH_COUNT) * 0.00038 * 3600.0) / (float(LPH_B - LPH_A) / 1000.0);
  LPH_COUNT = 0;
  LPH_A = millis();
  return val;
}

// Get Engine RPM --> Returns RPM of engine
float get_engine_rpm(void) {
  RPM_B = millis();
  float val = (float(RPM_COUNT) * 60.0) / (float(RPM_B - RPM_A) / 1000.0);
  RPM_COUNT = 0;
  RPM_A = millis();
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
