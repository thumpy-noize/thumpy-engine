/**
 * @file logger_helper.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Logger helper functions
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

enum LogLevel {
  NONE = 0,
  INFO = 1 << 1,
  DEBUG = 1 << 2,
  WARNING = 1 << 3,
  ERROR_LOG = 1 << 4,
  CRITICAL = 1 << 5,
  ALL = INFO | DEBUG | WARNING | ERROR_LOG | CRITICAL
};

/**
 * @brief Formats the message to the following:
 * [LOG_LEVEL] [DAY-MONTH-YEAR HOUR:MIN:SECOND]: MESSAGE
 * @param message
 * @param level
 * @return std::string
 */
std::string format_message( const std::string *message, LogLevel level );

/**
 * @brief Return log level as string
 *
 * @param level
 * @return std::string
 */
std::string log_level_to_string( LogLevel level );

/**
 * @brief Get current time as string
 * Format: [DAY-MONTH-YEAR HOUR:MIN:SECOND]
 * @return std::string
 */
std::string get_time_as_string();

}  // namespace Logger
}  // namespace Core
}  // namespace Thumpy
