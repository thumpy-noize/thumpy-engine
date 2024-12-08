
/**
 * @file logger_helper.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief logger_helper cpp file
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "logger_helper.hpp"

#include <chrono>
#include <ctime>
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

std::string format_message( const std::string *message, LogLevel level ) {
  return std::string( "[" + log_level_to_string( level ) + "] [" +
                      get_time_as_string() + "]: " + message->c_str() );
}

std::string log_level_to_string( LogLevel level ) {
  if ( ( level & INFO ) != 0 ) {
    return "INFO";
  }
  if ( ( level & DEBUG ) != 0 ) {
    return "DEBUG";
  }
  if ( ( level & WARNING ) != 0 ) {
    return "WARNING";
  }
  if ( ( level & ERROR_LOG ) != 0 ) {
    return "ERROR";
  }
  if ( ( level & CRITICAL ) != 0 ) {
    return "CRITICAL";
  }
  return "UNKNOWN";
}

std::string get_time_as_string() {
  std::string time_string;

  time_t current_time = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now() );

  std::time_t now = std::time( &current_time );
  std::tm *ptm = std::localtime( &now );
  char buffer[32];
  // Format: 02.12.2024 20:20:00
  std::strftime( buffer, 32, "%d-%m-%Y %H:%M:%S", ptm );
  return buffer;
}
}  // namespace Logger
}  // namespace Core
}  // namespace Thumpy
