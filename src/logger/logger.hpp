/**
 * @file logger.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Logs messages to multiple places
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
 * @brief Starts logger / creates log file
 *
 */
void init();

/**
 * @brief Closes logger / log file
 *
 */
void close_logger();

/**
 * @brief log a message
 *
 * @param message
 * @param level
 */
void log( const std::string &message, LogLevel level = INFO );

}  // namespace Logger
}  // namespace Core
}  // namespace Thumpy
