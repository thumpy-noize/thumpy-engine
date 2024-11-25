
#include "term_logger.hpp"
#include "logger_helper.hpp"
#include <iostream>
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

int term_log_vision = ALL;

void log_to_terminal(const std::string &message, LogLevel level) {
  if ((level & term_log_vision) != 0) {
    std::cout << terminal_color_from_level(level) << message << std::endl;
  }
}

std::string terminal_color_from_level(LogLevel level) {
  std::string color_code = "\033[0m"; // Default
  if ((level & CRITICAL) != 0) {
    return color_code.append("\033[41m\033[1m"); // Red highlighted Background
  }
  if ((level & ERROR) != 0) {
    return color_code.append("\033[31m"); // Red
  }
  if ((level & WARNING) != 0) {
    return color_code.append("\033[33m"); // Yellow
  }
  if ((level & INFO) != 0) {
    return color_code.append("\033[1m"); // Highlighted
  }
  if ((level & DEBUG) != 0) {
    return color_code; // Default
  }
  return color_code.append("\033[45m"); // None of the above - Purple background
}
} // namespace Logger
} // namespace Core
} // namespace Thumpy