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
    // explicit PumpController(const uint8_t &pwm_pin, float min_voltage, float max_voltage);
    explicit PumpController(const uint8_t &pwm_pin, const uint8_t &min_value, const uint8_t &max_value);
    void initPump();

    void setOutputPower(float level);
    void setOutputPower(uint8_t level);

    void setPowerForPeriod(float level, unsigned long ms);

    // TODO: think about getters for letting know system about change
    // ANSWEAR: Probably unnecessary - In this project, it's enough to just receive data, and letting know about errors
    // float getPowerPercantage();
    // uint8_t getMinValue();
    // uint8_t getMaxValue();
    
private:
    uint8_t _pwm_pin, _min_val, _max_val;
    uint8_t _current_level{0};
};

#endif  //PUMP_HPP