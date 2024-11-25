

#include "logger_helper.hpp"
#include <input_manager.hpp>
#include <logger.hpp>

bool DEBUG_MODE = true;

bool APLICATION_RUNNING = true;

int main() {
  Thumpy::Core::Logger::init();
  Thumpy::Core::Logger::log("Starting Engine...", Thumpy::Core::Logger::INFO);

  Thumpy::Core::IO::init();

  while (APLICATION_RUNNING) {

    Thumpy::Core::IO::poll_input();
  }

  return 0;
}
