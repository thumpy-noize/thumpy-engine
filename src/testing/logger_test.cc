
#include <gtest/gtest.h>

#include <fstream>
#include <ios>
#include <logger.hpp>
#include <string>

#include "file_logger.hpp"
#include "logger_helper.hpp"
#include "term_logger.hpp"

namespace Thumpy {
namespace Core {
// Demonstrate some basic assertions.
TEST( logger, test_file_logger_open ) {
  // Create file
  Logger::init();
  EXPECT_TRUE( Logger::get_log_file()->is_open() );

  // Close file
  Logger::close_log_file();
  EXPECT_FALSE( Logger::get_log_file()->is_open() );
}

TEST( logger, test_file_logger_write ) {
  std::string test_string = "test_message";

  // Create file
  Logger::init();
  EXPECT_TRUE( Logger::get_log_file()->is_open() );
  // Log to file
  Logger::log_to_file( test_string, Logger::LogLevel::DEBUG );

  // Close file
  Logger::close_log_file();
  EXPECT_FALSE( Logger::get_log_file()->is_open() );

  // Open File
  std::ifstream file_stream( Logger::log_path, std::ios::in );
  EXPECT_TRUE( file_stream.is_open() );

  // Read file
  std::string line;
  std::getline( file_stream, line );

  // Validate equal
  EXPECT_EQ( line, test_string );
}
}  // namespace Core

}  // namespace Thumpy
