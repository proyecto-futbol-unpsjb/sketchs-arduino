#include "Arduino.h"
#include <Servo.h>

Servo servo01;
int n;

void setup() {
  Serial.begin(9600);  
  servo01.attach(A1);
  servo01.write(90);
}

void loop() {
  if (Serial.available() > 0) {
    n = Serial.parseInt();
    if (n != 0) {
      servo01.write(n);
      Serial.println(n);
    }
  }
}
