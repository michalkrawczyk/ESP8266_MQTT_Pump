#include "Pump.hpp"

#include "Arduino.h"
#include <cassert>

#define WATCHDOG 1

namespace water_pump
{
    PumpController::PumpController(const uint8_t &pwm_pin, const uint8_t &min_value, const uint8_t &max_value):
    _pwm_pin(pwm_pin), _min_val(min_value), _max_val(max_value) 
    {
        // Check if provided settings are valid
        assert(min_value <= max_value && "PumpController: minimum value is bigger than maximum");
        assert(min_value >= 0 && "PumpController: Minimum should be in range (0-255)");
        assert(max_value <= 255 && "PumpController: Maximum should be in range (0-255)");
    }

    void PumpController::initPump()
    {
        /** @brief Initialize Pin for pump and set initial state */
        pinMode(_pwm_pin, OUTPUT);

        #ifdef DEBUG
            Serial.print("Pump inited at pin: ");
            Serial.println(_pwm_pin);
        #endif //DEBUG

        delay(10);
        analogWrite(_pwm_pin, 0);
    }

    void PumpController::setOutputPower(float level)
    {
        /** @brief  Set power on pump. 
         * @param level - Output power (0 - 100%)
         */

        level = (level > 0.0) ? level : 0.0;
        level = (level < 100.0) ? level : 100.0;

        uint8_t new_level(0U);

        if (level != 0.0)
        {
            new_level = static_cast<uint8_t>(
                (level * (_max_val - _min_val)) / 100 + _min_val);
        }

        if (new_level != _current_level)
        {
            analogWrite(_pwm_pin, new_level);
            _current_level = new_level;
        }
        

        #ifdef DEBUG
            Serial.print("Power set to: ");
            Serial.print(level);
            Serial.println('%');
        #endif //DEBUG

        #ifdef WATCHDOG
            ESP.wdtFeed();
        #endif //WATCHDOG
    }

    void PumpController::setPowerForPeriod(float level, unsigned long ms)
    {
        /** @brief  Set power on pump for given period. 
         * @param level - Output power (0 - 100%)
         * @param ms - Time of action in miliseconds
         */

        unsigned long begin_time = millis();
        unsigned long current_time = millis();

        setOutputPower(level);

        while (begin_time - current_time >= ms)
        {
            #ifdef WATCHDOG
                ESP.wdtFeed();
            #endif //WATCHDOG

            current_time = millis();
        }

        setOutputPower(0U);
    }
}