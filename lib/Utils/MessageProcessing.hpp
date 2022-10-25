#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace msg_processing
{
    struct Message;
    Message extractFloatMessage(std::string s);
    // uint64_t getTimeFromString(std::string s);

    // template <typename T> std::string to_string(const T &n);
    int stringToInt(std::string s);
}

struct msg_processing::Message
{
    float value;
    bool is_valid;
};


#endif  //UTILS_HPP