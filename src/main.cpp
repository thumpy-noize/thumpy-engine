

#include <input_manager.hpp>
#include <logger.hpp>
#include <window_manager.hpp>

bool DEBUG_MODE = true;

bool APPLICATION_RUNNING = true;
Thumpy::Core::Windows::WindowManager *window_manager;

int main() {
  Thumpy::Core::Logger::init();
  Thumpy::Core::Logger::log("Starting Engine...", Thumpy::Core::Logger::INFO);

  Thumpy::Core::IO::init();

  window_manager = new Thumpy::Core::Windows::WindowManager();

  // while (APPLICATION_RUNNING) {
  //   Thumpy::Core::IO::poll_input();
  // }

  // Terminate systems
  window_manager->terminate();

  // Dump log file
  Thumpy::Core::Logger::close_logger();

  return 0;
}
