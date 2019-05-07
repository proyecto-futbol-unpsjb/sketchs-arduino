#include "Arduino.h"

// Ping Sensor
#include <Ping.h>

// Ping Sensor
PingSensor ping(A1); // S1

long measure;

void setup() {
  Serial.begin(9600);    
}

void loop() {
  measure = ping.measureCM();
  Serial.println(measure);
  delay(1000);
}
