/**
 * @file file_logger.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Used to log messages to a file
 * @version 0.1
 * @date 2024-12-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <fstream>

#include "logger_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

const std::string log_path = "log.dump";

/**
 * @brief Create & open log file
 *
 */
void start_log_file();

/**
 * @brief Close log file
 *
 */
void close_log_file();

/**
 * @brief Log message to file
 *
 * @param message
 * @param level
 */
void log_to_file( const std::string &message, LogLevel level );

/**
 * @brief Get log file path
 *
 * @return std::ofstream*
 */
std::ofstream *get_log_file();

}  // namespace Logger

}  // namespace Core

}  // namespace Thumpy
