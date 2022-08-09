#ifndef PUMP_HPP
#define PUMP_HPP

#ifndef uint8_t
#include <stdint.h>  //has also uint16_t and uint32_t
#endif  //uint8_t

namespace water_pump
{
    class PumpController;
}

class water_pump::PumpController final
{
public:
    explicit PumpController(const uint8_t &pwm_pin, const uint8_t &min_value, const uint8_t &max_value);
    void initPump();

    void setOutputPower(float level);

    void setPowerForPeriod(float level, unsigned long ms);
    
private:
    uint8_t _pwm_pin, _min_val, _max_val;
    uint8_t _current_level{0};
};

#endif  //PUMP_HPP