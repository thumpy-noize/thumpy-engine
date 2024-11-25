#pragma once

#include "logger_helper.hpp"
#include <fstream>

namespace Thumpy {
namespace Core {
namespace Logger {

const std::string log_path = "log.dump";

void start_log_file();
void close_log_file();
void log_to_file(const std::string &message, LogLevel level);
std::ofstream *get_log_file();

} // namespace Logger

} // namespace Core

} // namespace Thumpy