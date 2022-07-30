#include "Pump.hpp"

#include "Arduino.h"
#include <cassert>
#define DEBUG 1

namespace water_pump
{
    PumpController::PumpController(const uint8_t &pwm_pin, const uint8_t &min_value, const uint8_t &max_value):
    _pwm_pin(pwm_pin), _min_val(min_value), _max_val(max_value) 
    {
        assert(min_value <= max_value && "PumpController: minimum value is bigger than maximum");
        assert(min_value >= 0 && "PumpController: Minimum should be in range (0-255)");
        assert(max_value <= 255 && "PumpController: Maximum should be in range (0-255)");
    }

    void PumpController::initPump()
    {
        pinMode(_pwm_pin, OUTPUT);
        #ifdef DEBUG
            Serial.print("Pump inited at pin: ");
            Serial.println(_pwm_pin);
        #endif
    }

    void PumpController::setOutputPower(float level)
    {
        level = (level > 0.0) ? level : 0.0;
        level = (level < 100.0) ? level : 100.0;

        if (level != _current_level)
        {
            uint8_t calculated_value = static_cast<uint8_t>(
                level * (_max_val - _min_val) + _min_val
            );
            analogWrite(_pwm_pin, calculated_value);
            _current_level = static_cast<uint8_t>(level);
        }

        #ifdef DEBUG
            Serial.print("Power set to: ");
            Serial.print(level);
            Serial.println('%');
        #endif
    }

    void PumpController::setOutputPower(uint8_t level)
    {
        level = (level >= _min_val) ? level : 0U; // Turn off when below minimum level
        level = (level < _max_val) ? level : _max_val;

        if (level != _current_level)
        {
            analogWrite(_pwm_pin, level);
            _current_level = level;
        }

        #ifdef DEBUG
            float new_level = ((level - _min_val) / (_max_val - _min_val)) * 100.0;
            Serial.print("Power set to: ");
            Serial.print(new_level);
            Serial.println('%');
        #endif
    }
}