
#include "window.hpp"

#include <cstddef>

#include "iostream"
#include "logger_helper.hpp"
#include "window_manager.hpp"
namespace Thumpy {
namespace Core {
namespace Windows {

Window::Window( std::string title ) {
  title_ = title;
  init_window();
}

void Window::init_window() {
  Logger::log( "Constructing window...", Logger::DEBUG );
  if ( !glfwInit() ) {
    Logger::log( "Failed to initialize glfw.", Logger::ERROR_LOG );
    throw GLFWNotCompatible( "Failed to initialize glfw.\n" );
  }

  glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
  glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

  window_ = glfwCreateWindow( WIDTH, HEIGHT, title_.c_str(), nullptr, nullptr );

  if ( window_ == NULL ) {
    Logger::log( "Failed to create window!", Logger::ERROR_LOG );
    throw GLFWNotCompatible( "Failed to create window!\n" );
  } else {
    Logger::log( "Constructed window...", Logger::DEBUG );
  }

  glfwSetWindowUserPointer( window_, this );
  glfwSetFramebufferSizeCallback( window_, framebuffer_resize_callback );
}

void Window::deconstruct_window() {
  if ( window_ == NULL ) {
    Logger::log( "Trying to destroy null window...", Logger::INFO );
    return;
  }

  Logger::log( "Destroying window - " + title_, Logger::INFO );
  glfwDestroyWindow( window_ );
}

void Window::loop() {
  // glfwSwapBuffers(window_); ??? maybe

  // glfwMakeContextCurrent(window_);
  // // glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::should_close() { return glfwWindowShouldClose( window_ ); };

void Window::framebuffer_resize_callback( GLFWwindow *window, int width, int height ) {
  // Logger::log( "Resizing framebuffer...", Logger::DEBUG );
  auto app = reinterpret_cast<Window *>( glfwGetWindowUserPointer( window ) );
  app->framebufferResized = true;
}

}  // namespace Windows

}  // namespace Core

}  // namespace Thumpy
