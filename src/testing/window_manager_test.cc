
#include "window_manager.hpp"

#include <gtest/gtest.h>

#include <fstream>
#include <ios>
#include <logger.hpp>
#include <string>

#include "logger_helper.hpp"

bool APPLICATION_RUNNING = true;

namespace Thumpy {
namespace Core {
namespace Windows {

#pragma region Window manager

class WindowManagerTest : public testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  WindowManagerTest() {
    // You can do set-up work for each test here.
  }

  ~WindowManagerTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).

    window_manager = new Thumpy::Core::Windows::WindowManager(
        Thumpy::Core::Windows::RenderAPI::NONE );

    EXPECT_TRUE( APPLICATION_RUNNING );
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    // Terminate systems
    window_manager->terminate();
  }

  Thumpy::Core::Windows::WindowManager *window_manager;

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

// Demonstrate some basic assertions.
TEST_F( WindowManagerTest, setup_and_teardown ) {}

TEST_F( WindowManagerTest, loop_100 ) {
  int i = 100;
  while ( APPLICATION_RUNNING && i != 0 ) {
    // Update window manager
    window_manager->loop();
    i--;
  }
}

#pragma endregion

#pragma region Vulkan
class WindowManagerVulkanTest : public testing::Test {
 protected:
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  WindowManagerVulkanTest() {
    // You can do set-up work for each test here.
  }

  ~WindowManagerVulkanTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).

    window_manager = new Thumpy::Core::Windows::WindowManager(
        Thumpy::Core::Windows::RenderAPI::VULKAN );

    EXPECT_TRUE( APPLICATION_RUNNING );
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    // Terminate systems
    window_manager->terminate();
  }

  Thumpy::Core::Windows::WindowManager *window_manager;

  // Class members declared here can be used by all tests in the test suite
  // for Foo.
};

// Demonstrate some basic assertions.
TEST_F( WindowManagerVulkanTest, setup_and_teardown ) {}

TEST_F( WindowManagerVulkanTest, loop_100 ) {
  int i = 100;
  while ( APPLICATION_RUNNING && i != 0 ) {
    // Update window manager
    window_manager->loop();
    i--;
  }
}
#pragma endregion

}  // namespace Windows

}  // namespace Core

}  // namespace Thumpy