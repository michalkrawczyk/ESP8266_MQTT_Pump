#include <Arduino.h>

#include "Pump.hpp"
#include "MQTT.hpp"
#include "RTC.hpp"
#include "MessageProcessing.hpp"

#include <stdio.h>

//TODO: Switch dev_id to string?

#define CONN_LED D4
#define PUMP_PIN D1

using RtcErr = rtc::ErrorCode;

water_pump::PumpController pump(PUMP_PIN, 170, 255); // 8-12V Range for 12V Pump
connection::MqttListenDevice dev(5, "feeds/test");


void sub_callback(char *data, uint16_t len)
{
  /* @brief: Subcribe Callback on received message
      for performing actions if message is valid and addresed for this device
  */
  #ifdef DEBUG
    Serial.print("Message received: ");
    Serial.println(data);
  #endif

  if ((len < 6) || (len > 22))
  {
    // id(1-3 digits) + '.'(separator) + cmd(3 chars) + value&time((1 - 6 char val + time 9 chars ['t' + hh:mm:ss]))
    return; // Invalid Length 
  }

  int dev_id {-1};
  sscanf(data, "%3i%*1s", &dev_id);

  if (!dev.compareID(dev_id))
  {
    // dev_id should be still -1 if sscanf fails
    return; // Invalid ID
  }

  uint16_t pos{0U};
  while (isdigit(data[pos]) && (pos < 2))
  {
    // id(1-3 digits) + '.'(separator)
    // get position of last id number to get cmd
    // TODO: rewrite on scanf (cmd issue)?
    pos++;
  }

  if((len - pos - 1) < 4)
  {
    // ((len - pos - 1) < 4) -> at least 4 chars for command and value

    #ifdef DEBUG
      Serial.println("Invalid Message");
    #endif

    return;
  }

  // Store command and value in strings for easier processing
  std::string cmd(data + (pos + 1), data + (pos + 4));
  std::string val(data + (pos + 4), data + len);

  #ifdef DEBUG
    Serial.println(cmd.c_str());
    Serial.print("Value:");
    Serial.println(val.c_str());
  #endif

  uint64_t time_ms{0};
   
  if (cmd == "set")
  {
    // Set Output Value
    unsigned short int h{0U}, m{0U}, s{0U};
    uint8_t res;
    float msg_value{0.0F};

    #ifdef SHORT_DURATION
      // 6 chars for float, 2x2 unsigned short int after 't' (mm:ss)
      res = sscanf(val.c_str(), "%6ft%2hu:%2hu", &msg_value, &m, &s);
    #else
      // 6 chars for float, 3x2 unsigned short int after 't' (hh:mm:ss)
      res = sscanf(val.c_str(), "%6ft%2hu:%2hu:%2hu",  &msg_value, &h, &m, &s);
    #endif

    if (res == 1)
    {
      // Time not found - set only value
      pump.setOutputPower(msg_value);
    }
    else if (res > 2)
    {
      time_ms = msg_processing::calculateTimeMs(h, m, s);
      #ifdef DEBUG
        Serial.print("Time:");
        Serial.println(long(time_ms));
      #endif

      pump.setPowerForPeriod(msg_value, time_ms);
    }

  }
  else if (cmd == "slp")
  {
    // Sleep
    unsigned short int h{0}, m{0}, s{0};

    if (sscanf(val.c_str(), "%hu:%2hu:%2hu", &h, &m, &s) == 3)
    {
      // format hh:mm:ss
      // go to deep sleep for given time
      time_ms = msg_processing::calculateTimeMs(h, m, s);

      RTC.deepSleep(time_ms * 1000);
    }
  }
  else
  {
    // Inform about invalid message
    dev.sendError(connection::SignalCode::BAD_PAYLOAD);
  }

}


void setup() {
  Serial.begin(9600);

  while(!Serial);

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

  #ifdef DEBUG
    Serial.println("Connected");
  #endif //DEBUG
  
  dev.init();

  connection::connectMQTT(); //TODO: Think about block till connect
  

  if (!dev.sendAckMsg())
  {
    RTC.deepSleepErr(RtcErr::MQTT_PUB_FAIL);
  }

  digitalWrite(CONN_LED, HIGH);
}

void loop() {

  if (!connection::checkWiFiConn())
  {
    // Connection Broken
    RTC.deepSleepErr(RtcErr::NO_WIFI);
  }

  if(connection::connectMQTT(3))
  {
    // wait on listening for 30 s
    connection::mqtt.processPackets(30000UL); 
    dev.retainConnection(); //TODO: Think about sleep
  }
  else
  {
    // MQTT Connection Broken

    #ifdef DEBUG
      Serial.println("No mqtt");
    #endif //DEBUG

    RTC.deepSleepErr(RtcErr::NO_MQTT);
  }
}