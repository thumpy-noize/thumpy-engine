
#include <gtest/gtest.h>

#include <fstream>
#include <ios>
#include <logger.hpp>
#include <string>

#include "file_logger.hpp"
#include "logger_helper.hpp"
#include "term_logger.hpp"
#include <filesystem>

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

  // Open File
  std::ifstream file_stream( std::filesystem::current_path().string() + "\\" + Logger::log_path, std::ios::in );
  EXPECT_TRUE( file_stream.is_open() );

  // Go to EOF
  file_stream.seekg( -4, std::ios_base::end );

  // Set max seeks
  int max_seeks = 100;
  
  // Seek back to "\n"
  while ( true ) {
    // Get current char
    char character;
    file_stream.get( character );

    // If begining of file, break out
    if ( (int)file_stream.tellg() <= 1 ) {
      file_stream.seekg( 0 );
      break;
    }

    // If new line found, break out
    if ( character == '\n' ) {
        break;
    }

    // Seek backwards
    file_stream.seekg( -2, std::ios_base::cur );
    max_seeks--;

    // Check for max seeks
    if(max_seeks <= 0)
    {
      EXPECT_TRUE(max_seeks > 0);
    }
  }
  
  // Read file
  std::string line;
  std::getline( file_stream, line );

  // Close ifstream
  file_stream.close();

  // Close logger file
  Logger::close_log_file();
  EXPECT_FALSE( Logger::get_log_file()->is_open() );

  // Validate equal
  EXPECT_EQ( line, test_string );
}
}  // namespace Core

}  // namespace Thumpy
