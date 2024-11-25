#pragma once

#include "logger_helper.hpp"

namespace Thumpy {
namespace Core {
namespace Logger {

void start_log_file();
void close_log_file();
void log_to_file(const std::string &message, LogLevel level);

} // namespace Logger

} // namespace Core

} // namespace Thumpy