#ifndef MQTT_H
#define MQTT_H


/************************************************
 *  Libraries
 ***********************************************/
#include "Network_Settings.h" // this file contains Network and MQTT Setup Settings
/* Note: That file includes: 
*(#define) WLAN_SSID, WLAN_PASS, IO_SERVER, IO_PORT, IO_USER, IO_KEY, IO_ERROR_FEED
*/

#ifndef Adafruit_MQTT_Client
#include <Adafruit_MQTT_Client.h>
#endif

#ifndef WiFiClient
#include <ESP8266WiFi.h>
#endif

#ifndef Adafruit_MQTT_Publish
#include <Adafruit_MQTT.h>
#endif

#ifndef uint8_t
#include <stdint.h>  //has also uint16_t and uint32_t
#endif  //uint8_t
 /***********************************************/

namespace connection
{
    extern WiFiClient client;
    extern Adafruit_MQTT_Client mqtt;

    enum class SignalCode : uint8_t;

    extern SignalCode last_signal;

    #ifdef ESP8266
    inline namespace Esp
    {
        void initWiFi();

        bool checkWiFiConn(uint8_t max_retries = CONN_RETRIES);
        bool connectMQTT(uint8_t retries = CONN_RETRIES);

        class MqttListenDevice;
    }// namespace Esp
    #endif //ESP8266
    
    
}

enum class connection::SignalCode : uint8_t
{
    NO_ERROR = 0u,
    BAD_PAYLOAD,            //For Incorrect Data in Payload
    BAD_PAYLOAD_SIZE,       //For Incorrect Number of Variables
    BAD_DEVICE_ID,          //For Commands that are for other Devices
    UNKNOWN_ERROR

};

#ifdef ESP8266

class connection::Esp::MqttListenDevice final
{
    public:
        explicit MqttListenDevice(const uint8_t &device_id, const std::string &feed_name);
        void init();
        bool sendError(const SignalCode &err_code);
        bool sendAckMsg();

        bool retainConnection();
        const bool compareID(const uint8_t &device_id) const;
        const std::string getLastMsg() const;

    private:
        const uint8_t _k_device_id;
        std::string _last_msg;
        
        Adafruit_MQTT_Publish _error_feed{&mqtt, IO_USER IO_ERROR_FEED};
        Adafruit_MQTT_Subscribe _listen_feed;

};


#endif  //ESP8266


#endif  //MQTT_H