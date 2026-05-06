
#include "logger_helper.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN

#include "logger.hpp"
#include "vulkan/examples/vulkan_mesh_example.hpp"
// #include "vulkan/examples/vulkan_triangle_example.hpp"
#include "vulkan/vulkan_window.hpp"
#include "window_manager.hpp"

extern bool APPLICATION_RUNNING;

namespace Thumpy {
namespace Core {
namespace Windows {

WindowManager::WindowManager( RenderAPI api ) {
  create_new_window( api );
  // loop();
}

WindowManager::~WindowManager() { terminate(); }

void WindowManager::create_new_window( RenderAPI api, std::string title ) {
  Logger::log( "Constructing new window...", Logger::INFO );

  // Window* window;
  switch ( api ) {
    case VULKAN: {
      Logger::log( "Using Vulkan.", Logger::INFO );

      // Create vulkan window
      windows_.push_back( std::make_unique<Vulkan::Examples::VulkanMeshExample>( title ) );
      // Init vulkan after construction (insures we use overriden functions)
      dynamic_cast<Vulkan::VulkanWindow*>( windows_.back().get() )->init_vulkan();
      break;
    }

    case NONE:
    default: {
      Logger::log( "No rendering api selected.", Logger::WARNING );

      // Create empty glfw window
      windows_.push_back( std::make_unique<Window>( title ) );
    }
  }
}

void WindowManager::loop() {
  // Check if we still have open windows
  if ( windows_.empty() ) {
    // Close application when all windows are closed
    APPLICATION_RUNNING = false;
    return;
  }

  // Poll input events
  glfwPollEvents();

  // Loop threw all windows
  for ( int i = windows_.size() - 1; i >= 0; i-- ) {
    // Check for window exit signal
    if ( windows_.at( i )->should_close() ) {
      // Delete Window
      windows_.erase( windows_.begin() + i );
      // Resize i
      continue;
    }

    // glfwMakeContextCurrent(windows_.at(i)); ??? maybe

    // run window loop
    windows_.at( i )->loop();

    // // glClear(GL_COLOR_BUFFER_BIT);
    // glfwSwapBuffers(windows_.at(i));
  }  // get next window and repeat
}

void WindowManager::terminate() {
  // Validate windows are closed
  if ( windows_.size() != 0 ) {
    Logger::log( "Terminating existing windows...", Logger::INFO );
    windows_.clear();
  }

  Logger::log( "Terminating glfw...", Logger::INFO );
  glfwTerminate();
}

}  // namespace Windows

}  // namespace Core

}  // namespace Thumpy
