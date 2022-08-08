#include <Arduino.h>

#include "Pump.hpp"

#define CONN_LED D4
#define PUMP_PIN D1

water_pump::PumpController pump(PUMP_PIN, 170, 255); // 8-12V Range for 12V Pump

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);
  Serial.println("Start");
  pump.initPump();
}

void loop() {
  // digitalWrite(MCU_LED, HIGH);
  // delay(1000);
  // digitalWrite(MCU_LED, LOW);
  // delay(1000);
  // put your main code here, to run repeatedly:
}