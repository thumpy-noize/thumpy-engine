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
  log_file->open( log_path, std::ofstream::out | std::ofstream::app );
  if ( !log_file->is_open() ) {
    log( "Log file failed to open.", ERROR_LOG );
  }
}

void close_log_file() {
  trim_log_file( 100000 );  // TODO: Finish trim and use variable

  // Close file
  if ( log_file->is_open() ) {
    log( "Dumping log to " + std::filesystem::current_path().string() + "/" + log_path, DEBUG );
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

void trim_log_file( uint32_t max_size ) {
  // Ignore trim is size is 0
  if ( max_size == 0 ) {
    return;
  }

  // Compare current file size to max size
  int64_t log_size = static_cast<int64_t>( std::filesystem::file_size( log_path ) );

  int64_t trim = log_size - static_cast<int64_t>( max_size );

  if ( trim <= 0 ) {
    // Log is not at max size, skiping trim
    return;
  }

  log( "Trimming log...", DEBUG );
  log( "LogSize: " + std::to_string( log_size ), DEBUG );
  log( "MaxSize: " + std::to_string( max_size ), DEBUG );
  log( "Trim: " + std::to_string( trim ), DEBUG );

  // TODO: Implement auto trim
  // We will need to open the file in write or out mode.
  log( "The log file has exceeded its maximum size. But automatic trim is not yet implemented.",
       WARNING );
}

}  // namespace Logger

}  // namespace Core

}  // namespace Thumpy
