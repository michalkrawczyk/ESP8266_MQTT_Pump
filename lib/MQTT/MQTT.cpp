#include "MQTT.hpp"
// #include "Esp.h"
#include <sstream>
#include "RTC.hpp"


/************************************************
 *  Callback
 ***********************************************/

#include "Pump.hpp"

extern void sub_callback(char *data, uint16_t len);


/************************************************
 *  Support functions - Declarations & Templates
 ***********************************************/

static std::string feedToString(const std::string &feed_name);

template <typename T>
static std::string to_string(const T &n)
{
    /* @brief: Casting to string
        Note: Compiler Bug causes not finding std::to_string even in <string>
    */
    std::ostringstream out_stream ;
    out_stream<< n ;

    return out_stream.str() ;
}

/************************************************
 *  MQTT & WiFi Connection
 ***********************************************/

namespace connection{
    extern WiFiClient client{};
    extern Adafruit_MQTT_Client mqtt(&client,
                                    IO_SERVER,
                                    IO_PORT,
                                    IO_USER,
                                    IO_KEY);

    extern SignalCode last_signal{SignalCode::NO_ERROR};

#ifdef ESP8266
    inline namespace Esp
    {
        void initWiFi()
        {
            // @brief: Init Wifi Connection

            #ifdef DEBUG
                Serial.println("Connecting with Network");
            #endif// DEBUG
            //Static Ip Connection is much faster in ESP than DHCP

            if(!WiFi.forceSleepWake())
            {
                // Wake wi-fi modem if was off after sleep
                wifi_station_connect();
            } 

            delay(1);

            //1 and 3 have STA enabled - According to Wifi.waitForConnectResult()
            // if((wifi_get_opmode() & 1) == 0) 
            // {
            //     WiFi.enableSTA(true); 
            //     delay(10);
            // }
            WiFi.enableSTA(true); 
            delay(10);
            WiFi.mode(WIFI_STA);
            delay(10);
            
            WiFi.persistent(false); //Do not store data in flash

            // IPAddress ip(IP_ADDR), gateway(GATEWAY_ADDR), subnet(SUBNET_ADDR);
            // WiFi.config(ip, gateway, subnet);
            // Issue with WiFi config - despite same id, network not working properly

            if(RTC.isValid())             
            {
                // The RTC data was good, make a quick connection
                Serial.println("RTC");
                auto rtc_data = RTC.getData();

                WiFi.begin(WLAN_SSID, WLAN_PASS, rtc_data.channel, rtc_data.bssid, true);
            }
            else 
            {
                // The RTC data was not valid, so make a regular connection
                WiFi.begin(WLAN_SSID, WLAN_PASS);
            }
        }

        bool checkWiFiConn(uint8_t max_retries)
        {
            // @brief: Check WiFi connection and reconnect if disconnected
            while(WiFi.status() != WL_CONNECTED && max_retries--)
            {
                delay(100);

                #ifdef DEBUG
                    Serial.print('.');
                #endif //DEBUG
            }
            if(WiFi.status() == WL_CONNECTED)
            {
                #ifdef DEBUG
                    Serial.println("Fast Conn");
                #endif// DEBUG
                return true;
            }

            #ifdef DEBUG
                Serial.println("Fast Connect Failed - Try normal");
            #endif// DEBUG

            // Unable to reconnect - Reset connection and radio
            WiFi.disconnect();
            delay(10);
            WiFi.forceSleepBegin();
            delay(10);
            WiFi.forceSleepWake();
            delay(10);
            WiFi.begin(WLAN_SSID, WLAN_PASS);           

            return WiFi.waitForConnectResult(6000) == WL_CONNECTED;
        }

        bool connectMQTT(uint8_t retries)
        {
            if (mqtt.connected()) 
            {
                return true;
            }

            Serial.print(F("Connecting with MQTT... "));

            int8_t ret;
            
            while (ret = mqtt.connect()) // mqtt.connect will return 0 if connected
            { 
                #ifdef DEBUG
                    Serial.println(mqtt.connectErrorString(ret));
                    Serial.println(int(ret));
                #endif //DEBUG

                Serial.println("Retrying MQTT connection in 5 seconds...");
                mqtt.disconnect();

                delay(5000);
                retries--;
                

                if (!retries) 
                {
                    return false;
                }
            }
            Serial.println(F("Connected!"));
            return true;
        }
        
        MqttListenDevice::MqttListenDevice(const uint8_t &device_id, const std::string &feed_name):
        _k_device_id(device_id),
        _listen_feed(&mqtt, feedToString(feed_name).c_str())
        {
        }

        void MqttListenDevice::init()
        {
            // @brief: Init Device - Subscribe channel and init callbacks
            _listen_feed.setCallback(sub_callback);
            
            if (!mqtt.subscribe(&_listen_feed))
            {
                RTC.deepSleepErr(rtc::ErrorCode::MQTT_SUB_FAIL);
            }

            // Last Will in case of error on device
            mqtt.will(IO_ERROR_FEED, "-1");
        }

        bool MqttListenDevice::sendError(const SignalCode &err_code)
        {
            /* @brief: Send information about error to broker (to predefined channel)
                @param err_code - Error code to send (defined in MQTT.hpp)
            */
            
            std::string msg(to_string(static_cast<int>(_k_device_id)));
            //TODO: ADD KEY HERE
            msg += '/' + to_string(static_cast<int>(err_code));

            Serial.print("Signal Sent:");
            Serial.println(msg.c_str());
            
            return _error_feed.publish(msg.c_str());
        }

        bool MqttListenDevice::sendAckMsg()
        {
            // @brief: Send SignalCode::NO_ERROR as ACK message
            return sendError(SignalCode::NO_ERROR);
        }

        bool MqttListenDevice::retainConnection()
        {
            /* @brief: Ping to retain connection.
                If unable to ping - disconnect to enable reconnection
            */
            bool result(false);

            if (!(result = mqtt.ping()))
            {
                mqtt.disconnect();
            }

            return result;
        }

        const bool MqttListenDevice::compareID(const uint8_t &device_id) const
        {
            // @brief: Device ID validation
            return _k_device_id == device_id;
        }

        // const std::string MqttListenDevice::getLastMsg() const
        // {
        //     return _last_msg;
        // }
    } //inline namespace Esp

}//namespace Connection
#endif //ifdef ESP8266


/************************************************
 *  Support functions - Definitions
 ***********************************************/
    
static std::string feedToString(const std::string &feed_name)
{
    // @brief: Prepare feed channel
    if(!feed_name.empty() && feed_name[0] == '/')
    {
        return std::string(IO_USER + feed_name);
    }

    std::string feed(IO_USER);
    feed += '/' + feed_name;
    Serial.print("Feed:");
    Serial.println(feed.c_str());
    return feed;
}