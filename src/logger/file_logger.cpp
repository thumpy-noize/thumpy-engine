/**
 * @file file_logger.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief file_logger cpp file
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "file_logger.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "logger.hpp"
#include "logger_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

int file_log_vision = ALL;
std::ofstream *log_file = new std::ofstream();

void start_log_file() {
  // Open log file
  log_file->open( log_path, std::ofstream::out | std::ofstream::trunc );
  if ( !log_file->is_open() ) {
    log( "Log file failed to open.", ERROR_LOG );
  }
}

void close_log_file() {
  // Close file
  if ( log_file->is_open() ) {
    log( "Dumping log to " + std::filesystem::current_path().string() + "/" +
             log_path,
         DEBUG );
    log_file->close();
  }
}

void log_to_file( const std::string &message, LogLevel level ) {
  if ( log_file->is_open() ) {
    if ( ( level & file_log_vision ) != 0 ) {
      // Append to file
      *log_file << message << std::endl;
    }
  }
}

std::ofstream *get_log_file() { return log_file; }

}  // namespace Logger

}  // namespace Core

}  // namespace Thumpy
