

#include "logger.hpp"
#include <input_manager.hpp>
#include <signal.h>

namespace Thumpy {
namespace Core {
namespace IO {

static void finish(int sig);

void init() { (void)signal(SIGINT, finish); }

#pragma region Signal Handling

static void finish(int sig) {

  // Shutdown
  Thumpy::Core::Logger::log("Shuting down...", Thumpy::Core::Logger::INFO);
  Logger::close_logger();
  exit(0);
}

#pragma endregion Signal Handling

void poll_input() {}

} // namespace IO
} // namespace Core
} // namespace Thumpy
