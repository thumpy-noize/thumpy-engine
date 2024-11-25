
#include "logger_helper.hpp"
#include <chrono>
#include <ctime>
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

std::string format_message(const std::string *message, LogLevel level) {
  return std::string("[" + log_level_to_string(level) + "] [" +
                     get_time_as_string() + "]: " + message->c_str());
}

std::string log_level_to_string(LogLevel level) {
  if ((level & INFO) != 0) {
    return "INFO";
  }
  if ((level & DEBUG) != 0) {
    return "DEBUG";
  }
  if ((level & WARNING) != 0) {
    return "WARNING";
  }
  if ((level & ERROR) != 0) {
    return "ERROR";
  }
  if ((level & CRITICAL) != 0) {
    return "CRITICAL";
  }
  return "UNKNOWN";
}

// Replace this when we have a proper clock
std::string get_time_as_string() {
  time_t current_time = std::chrono::system_clock::to_time_t(
      std::chrono::high_resolution_clock::now());
  std::string time_string = ctime(&current_time);
  time_string = time_string.substr(0, time_string.length() - 1);
  return time_string;
}
} // namespace Logger
} // namespace Core
} // namespace Thumpy
