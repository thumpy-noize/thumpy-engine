#include "file_logger.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

int file_log_vision = ALL;
std::ofstream *log_file = new std::ofstream();

void start_log_file() {
  // Open log file
  log_file->open(log_path, std::ofstream::out | std::ofstream::trunc);
  if (!log_file->is_open()) {
    log("Log file failed to open.", ERROR);
  }
}

void close_log_file() {
  // Close file
  if (log_file->is_open()) {

    log("Dumping log to " + std::filesystem::current_path().string() + "/" +
            log_path,
        DEBUG);
    log_file->close();
  }
}

void log_to_file(const std::string &message, LogLevel level) {
  if ((level & file_log_vision) != 0) {
    // Append to file
    *log_file << message << std::endl;
  }
}

std::ofstream *get_log_file() { return log_file; }

} // namespace Logger

} // namespace Core

} // namespace Thumpy
