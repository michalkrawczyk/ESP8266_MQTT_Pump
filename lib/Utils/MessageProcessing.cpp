#include "MessageProcessing.hpp"

#include <sstream>

namespace msg_processing
{
  uint64_t calculateTimeMs(const uint8_t h, uint8_t m, uint8_t s)
  {
    /** @brief  Calculates time in miliseconds for timed functions
    * @param h - time in hours
    * @param m - time in minutes
    * @param s - time in seconds
    */
    uint64_t total_time {0};

    // Clamp minutes and seconds to be in range [0 : 59]
    m = (m < 60) ? m : 59;
    s = (s < 60) ? s : 59;

    total_time += h * 3600000;
    total_time += m * 60000;
    total_time += s * 1000;

    return total_time;
  }
} // namespace msg_processing

