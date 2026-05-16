/**
 * @file vulkan_triangle.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Creates a window with a sierpinski's triangle
 * @version 0.1
 * @date 2024-12-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan/vulkan_window.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {
namespace Examples {

class VulkanMeshExample : public VulkanWindow {
 public:
  VulkanMeshExample( std::string title );

  ~VulkanMeshExample() override {};  // deconstruct_window(); };
  /**
   * @brief Setup vulkan
   *
   */
  // void init_vulkan() override;

  virtual void init_texture() override;

  virtual void init_mesh() override;

  virtual void init_shader() override;

  /**
   * @brief deconstruct vulkan window
   *
   */
  // void deconstruct_window() override;

  /**
   * @brief Render loop
   *
   */
  //   virtual void loop() override;
};

}  // namespace Examples
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
