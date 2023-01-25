#include <Arduino.h>

const int sensorIn = 34;

float getReading() {
  return analogRead(sensorIn) * 3.3 / 4096.;
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(getReading());
}
