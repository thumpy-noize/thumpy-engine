#include "logger.hpp"
#include "file_logger.hpp"
#include "term_logger.hpp"
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

void init() { start_log_file(); }

void close_logger() { close_log_file(); }

void log(const std::string &message, LogLevel level) {
  std::string moded_message = format_message(&message, level);
  log_to_file(moded_message, level);
  log_to_terminal(moded_message, level);
}

} // namespace Logger
} // namespace Core
} // namespace Thumpy
