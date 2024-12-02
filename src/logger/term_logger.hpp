/**
 * @file term_logger.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Used to log messages to the terminal
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <string>

#include "logger_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

/**
 * @brief log message to terminal
 *
 * @param message
 * @param level
 */
void log_to_terminal( const std::string &message, LogLevel level );

/**
 * @brief get color value string from log level
 *
 * @param level
 * @return std::string
 */
std::string terminal_color_from_level( LogLevel level );

}  // namespace Logger

}  // namespace Core

}  // namespace Thumpy
