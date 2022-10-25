
#include <sstream>

#include "MessageProcessing.hpp"

namespace msg_processing
{
  Message msg_processing::extractFloatMessage(std::string s) {
    /**
     * @brief  Extract float message from string
     * @param s - string containing number
     * 
     * @return Message struct with params:
     *  - value - float number
     *  - is_valid - information if string contains valid float number
     */
      std::istringstream iss(s);
      
      Message msg;

      iss >> std::noskipws >> msg.value;

      msg.is_valid = iss && iss.eof();

      return msg;
  }

  // template < typename T > std::string to_string(const T &n)
  // {
  //       std::ostringstream oss ;
  //       oss << n ;
  //       return oss.str() ;
  // }

  int stringToInt(std::string s)
  {
    /**
     * @brief  convert number in std::string to integer (due to mingw issue)
     * @param s - string containing number
     * 
     * @return extracted integer value if convertion was valid. Otherwise -1
     */
    std::istringstream iss(s);
      
      int num;

      iss >> num;

      return (iss && iss.eof()) ? num : -1;
  }
} // namespace msg_processing

