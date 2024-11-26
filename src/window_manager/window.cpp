
#include "window.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
namespace Thumpy {
namespace Core {
namespace Windows {

Window::Window(std::string title) {
  title_ = title;
  init_window();
}

Window::~Window() { deconstruct_window(); }

void Window::init_window() {
  Logger::log("Creating window...", Logger::INFO);
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(WIDTH, HEIGHT, title_.c_str(), nullptr, nullptr);
}

void Window::deconstruct_window() {
  if (window_ == NULL) {
    return;
  }
  Logger::log("Destroying window - " + title_ + "", Logger::INFO);
  glfwDestroyWindow(window_);
}

void Window::loop() {
  glfwMakeContextCurrent(window_);
  // glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window_);
}

bool Window::should_close() { return glfwWindowShouldClose(window_); };

} // namespace Windows

} // namespace Core

} // namespace Thumpy
