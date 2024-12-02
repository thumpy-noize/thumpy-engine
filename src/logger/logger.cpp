/**
 * @file logger.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief logger cpp file
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "logger.hpp"

#include <string>

#include "file_logger.hpp"
#include "logger_helper.hpp"
#include "term_logger.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

void init() { start_log_file(); }

void close_logger() { close_log_file(); }

void log( const std::string &message, LogLevel level ) {
  std::string modded_message = format_message( &message, level );
  log_to_file( modded_message, level );
  log_to_terminal( modded_message, level );

  if ( ( level & CRITICAL ) != 0 ) {
    throw std::runtime_error( message );
  }
}

}  // namespace Logger
}  // namespace Core
}  // namespace Thumpy
