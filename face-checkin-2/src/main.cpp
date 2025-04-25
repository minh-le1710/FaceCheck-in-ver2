#include <Arduino.h>

// Đổi LED_PIN nếu bạn muốn test pin khác
#ifndef LED_PIN
  #define LED_PIN 2    // LED on-board ESP32 DevKit thường là GPIO2
#endif

void setup() {
  // Thiết lập chân LED là OUTPUT
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Bật LED
  digitalWrite(LED_PIN, HIGH);
  delay(1900);

  // Tắt LED
  digitalWrite(LED_PIN, LOW);
  delay(10000);
}
