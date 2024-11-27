
#include "window.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include "window_manager.hpp"
#include <cstddef>
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

  window_ = glfwCreateWindow(WIDTH, HEIGHT, title_.c_str(), nullptr, nullptr);

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, framebuffer_resize_callback);
}

void Window::deconstruct_window() {
  if (window_ == NULL) {
    return;
  }
  Logger::log("Destroying window - " + title_ + "", Logger::INFO);
  glfwDestroyWindow(window_);
}

void Window::loop() {
  // glfwSwapBuffers(window_); ??? maybe

  // glfwMakeContextCurrent(window_);
  // // glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::should_close() { return glfwWindowShouldClose(window_); };

void Window::framebuffer_resize_callback(GLFWwindow *window, int width,
                                         int height) {
  auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
  app->framebufferResized = true;
}

} // namespace Windows

} // namespace Core

} // namespace Thumpy
