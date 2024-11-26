#pragma once

#include "logger_helper.hpp"
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

// Starts logger / creates log file
void init();

// Close logger
void close_logger();

// Log a message
void log(const std::string &message, LogLevel level = INFO);

} // namespace Logger
} // namespace Core
} // namespace Thumpy
