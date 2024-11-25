#pragma once
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {
enum LogLevel {
  NONE = 0,
  INFO = 1 << 1,
  DEBUG = 1 << 2,
  WARNING = 1 << 3,
  ERROR = 1 << 4,
  CRITICAL = 1 << 5,
  ALL = INFO | DEBUG | WARNING | ERROR | CRITICAL
};

std::string format_message(const std::string *message, LogLevel level);
std::string log_level_to_string(LogLevel level);
std::string get_time_as_string();

} // namespace Logger
} // namespace Core
} // namespace Thumpy