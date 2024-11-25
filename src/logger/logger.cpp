#include "logger.hpp"
#include "file_logger.hpp"
#include "term_logger.hpp"
#include <iostream>

#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

void init() {}

void close_logger() {}

void log(const std::string &message, LogLevel level) {
  std::string moded_message = format_message(&message, level);
  // log_to_file(moded_message, level);
  log_to_terminal(moded_message, level);
}

} // namespace Logger
} // namespace Core
} // namespace Thumpy