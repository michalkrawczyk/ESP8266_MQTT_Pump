#include "MessageProcessing.hpp"

#include <sstream>

namespace msg_processing
{
  uint64_t calculateTimeMs(uint8_t h, uint8_t m, uint8_t s)
  {
    uint64_t total_time {0};

    total_time += h * 3600000;
    total_time += m * 60000;
    total_time += s * 1000;

    return total_time;
  }
} // namespace msg_processing

