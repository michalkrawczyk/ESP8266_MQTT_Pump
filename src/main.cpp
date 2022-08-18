#include <Arduino.h>

#include "Pump.hpp"
#include "MQTT.hpp"
#include "RTC.hpp"

#define DEBUG 1
#define CONN_LED D4
#define PUMP_PIN D1

using RtcErr = rtc::ErrorCode;

water_pump::PumpController pump(PUMP_PIN, 170, 255); // 8-12V Range for 12V Pump
connection::MqttListenDevice dev(5, "feeds/test");

void setup() {
  Serial.begin(9600);

  while(!Serial);
  Serial.println("Start");

  pump.initPump();
  pinMode(CONN_LED, OUTPUT);
  
  connection::initWiFi();

  while(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(CONN_LED, LOW);

    delay(100);

    #ifdef DEBUG
      Serial.print('.');
    #endif //DEBUG

    digitalWrite(CONN_LED, HIGH);
  }

  Serial.println("Connected");
  dev.init();

  connection::connectMQTT(); //TODO: Think about block
  

  if (!dev.sendAckMsg())
  {
    RTC.deepSleepErr(RtcErr::MQTT_PUB_FAIL);
  }

}

void loop() {

  if (!connection::checkWiFiConn())
  {
    // Connection Broken
    RTC.deepSleepErr(RtcErr::NO_WIFI);
  }

  if(connection::connectMQTT(3))
  {
    connection::mqtt.processPackets(30000UL); //wait 30 s
    dev.retainConnection(); //TODO: Think about sleep
  }
  else
  {
    // MQTT Connection Broken
    Serial.println("No mqtt");
    // RTC.deepSleepErr(RtcErr::NO_MQTT);
  }
}