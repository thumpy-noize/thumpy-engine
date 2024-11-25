#pragma once

#include "logger_helper.hpp"
#include <string>

namespace Thumpy {
namespace Core {
namespace Logger {

/// log message to terminal
void log_to_terminal(const std::string &message, LogLevel level);

/// get color value string from log level
std::string terminal_color_from_level(LogLevel level);

} // namespace Logger

} // namespace Core

} // namespace Thumpy
