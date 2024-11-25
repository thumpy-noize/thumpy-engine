
#include "term_logger.hpp"
#include "logger_helper.hpp"
#include <iostream>

namespace Thumpy {
namespace Core {
namespace Logger {

int log_vision = ALL;

void log_to_terminal(const std::string &message, LogLevel level) {
  if ((level & log_vision) != 0) {
    std::cout << message << std::endl;
  }
}
} // namespace Logger
} // namespace Core
} // namespace Thumpy