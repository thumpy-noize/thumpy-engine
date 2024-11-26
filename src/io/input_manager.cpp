

#include "logger.hpp"
#include <input_manager.hpp>
#include <signal.h>

extern bool APPLICATION_RUNNING;

namespace Thumpy {
namespace Core {
namespace IO {

static void finish(int sig);

void init() { (void)signal(SIGINT, finish); }

#pragma region Signal Handling

static void finish(int sig) {

  // Shutdown
  Thumpy::Core::Logger::log("Shuting down by exit signal...",
                            Thumpy::Core::Logger::INFO);
  APPLICATION_RUNNING = false;
}

#pragma endregion Signal Handling

void poll_input() {}

} // namespace IO
} // namespace Core
} // namespace Thumpy
