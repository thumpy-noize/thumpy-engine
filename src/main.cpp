

#include <input_manager.hpp>
#include <logger.hpp>
#include <window_manager.hpp>

bool DEBUG_MODE = true;

bool APPLICATION_RUNNING = true;

Thumpy::Core::Windows::WindowManager *window_manager;

// TODO: Move engine to separate file
class Engine {
 public:
  Engine() { run(); };
  ~Engine() { terminate(); };

  void run() {
    init();
    loop();
  }

  void terminate() {
    Thumpy::Core::Logger::log( "Terminating engine...", Thumpy::Core::Logger::INFO );

    // Terminate systems
    delete window_manager;

    // Dump log file
    Thumpy::Core::Logger::close_logger();
  }

  void init() {
    Thumpy::Core::Logger::init();
    Thumpy::Core::Logger::log( "Starting Engine...", Thumpy::Core::Logger::INFO );

    Thumpy::Core::IO::init();

    window_manager =
        new Thumpy::Core::Windows::WindowManager( Thumpy::Core::Windows::RenderAPI::VULKAN );

    Thumpy::Core::Logger::log( "Setup complete...", Thumpy::Core::Logger::INFO );
  }

  void loop() {
    // Engine loop

    // TODO: Add fixed update
    // This will loop as fast as possible, witch is great a-lot of the time. But we will benefit
    // from a fixed updated

    while ( APPLICATION_RUNNING ) {
      // Thumpy::Core::IO::poll_input();

      // Update window manager
      window_manager->loop();
    }
  }
};

int main() {
  try {
    Engine engine = Engine();

  } catch ( const std::exception &e ) {
    Thumpy::Core::Logger::log( e.what(), Thumpy::Core::Logger::ERROR_LOG );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
