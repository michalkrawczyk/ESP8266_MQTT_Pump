#ifndef PUMP_HPP
#define PUMP_HPP

#ifndef uint8_t
#include <stdint.h>  //has also uint16_t and uint32_t
#endif  //uint8_t

#ifdef ESP8266
#define out_voltage_t uint16_t
#define MAX_OUT_VOLTAGE 1023
#else
#define out_voltage_t uint8_t
#define MAX_OUT_VOLTAGE 255
#endif

namespace water_pump
{
    class PumpController;
}

class water_pump::PumpController final
{
public:
    explicit PumpController(const uint8_t &pwm_pin, const out_voltage_t &min_value, const out_voltage_t &max_value);
    void initPump();

    void setOutputPower(float level);
    void setPowerForPeriod(float level, unsigned long ms);
    
private:
    uint8_t _pwm_pin;
    out_voltage_t _min_val, _max_val;
    out_voltage_t _current_level{0};
    uint8_t _pump_block_flag{0};
};

#endif  //PUMP_HPP
