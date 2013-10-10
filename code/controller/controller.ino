/*
  Controller.ino
*/

int BAUD = 9600;
char COMMAND;

void setup() {
  Serial.begin(BAUD);
}

void loop() {
  COMMAND = Serial.read();
}
