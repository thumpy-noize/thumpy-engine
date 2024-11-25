#include "file_logger.hpp"
#include "logger_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

int log_vision = ALL;

void start_log_file() {
  // Open a file
}

void close_log_file() {
  // Close file
}

void log_to_file(const std::string &message, LogLevel level) {
  if ((level & log_vision) != 0) {
    // Append to file...
  }
}

} // namespace Logger

} // namespace Core

} // namespace Thumpy