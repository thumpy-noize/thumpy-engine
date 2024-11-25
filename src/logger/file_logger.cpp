#include "file_logger.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include <fstream>

namespace Thumpy {
namespace Core {
namespace Logger {

int file_log_vision = ALL;
std::ofstream log_file;

std::string log_path = "log.dump";

void start_log_file() {
  // Open log file
  log_file.open(log_path, std::ofstream::out | std::ofstream::trunc);
  if (!log_file.is_open()) {
    log("Log file failed to open.", ERROR);
  }
}

void close_log_file() {
  // Close file
  if (log_file.is_open()) {
    log("Dumping log to " + log_path, INFO);
    log_file.close();
  }
}

void log_to_file(const std::string &message, LogLevel level) {
  if ((level & file_log_vision) != 0) {
    // Append to file
    log_file << message << std::endl;
  }
}

} // namespace Logger

} // namespace Core

} // namespace Thumpy