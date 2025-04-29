#include <Arduino.h>

#ifndef LED_PIN
  #define LED_PIN 2    
#endif

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(1900);

  digitalWrite(LED_PIN, LOW);
  delay(10000);
}
