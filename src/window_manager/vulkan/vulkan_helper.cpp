
#include "vulkan_helper.hpp"
#include "logger.hpp"
#include "logger_helper.hpp"
#include <GLFW/glfw3.h>
#include <cstring>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

bool check_validation_layer_support() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char *layerName : validationLayers) {
    bool layerFound = false;

    for (const auto &layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

std::vector<const char *> get_required_extensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

uint32_t find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                          VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  Logger::log("failed to find suitable memory type!", Logger::CRITICAL);
  return 0;
}

#pragma region Shape generation

namespace Shapes {

/**
 * @brief Creates a sierpinski triangle from list of triangles
 * shout out to jacob keller for the guidance -
 * (https://www.vfxkeeler.com/post/vulkan-sierpinski-triangle)
 * @param recursions number of times to fractal
 * @param startingTriangle starting triangles
 * @return std::vector<Vertex> sierpinski triangles
 */
std::vector<Vertex>
generate_sierpinski_triangle(uint32_t recursions,
                             std::vector<Vertex> startingTriangle) {

  Logger::log("Generating sierpinski - recursions left: " +
                  std::to_string(recursions) + "",
              Logger::INFO);

  std::vector<Vertex> nextTriangle;

  // Create a new triangle for each vertex
  for (int i = 0; i < startingTriangle.size(); i++) {

    int vertMod = i % 3; // Vertex position in triangle
    int nextIndex;
    int lastIndex;
    Vertex lastVertex;
    Vertex nextVertex;
    switch (vertMod) {
    case 0: // if A get B & C
      nextIndex = i + 1;
      lastIndex = i + 2;
      break;
    case 1: // if B get C & A
      nextIndex = i + 1;
      lastIndex = i - 1;
      break;
    case 2: // if C get B & A
      nextIndex = i - 2;
      lastIndex = i - 1;
      break;
    default:
      nextIndex = i;
      lastIndex = i;
    }

    // get midpoint between this vertex & next index
    nextVertex = Vertex::mid(startingTriangle[nextIndex], startingTriangle[i]);
    // get midpoint between this vertex & last index
    lastVertex = Vertex::mid(startingTriangle[lastIndex], startingTriangle[i]);

    nextTriangle.push_back(nextVertex);          // midpoint
    nextTriangle.push_back(lastVertex);          // midpoint
    nextTriangle.push_back(startingTriangle[i]); // this vertex
  }

  recursions--;
  if (recursions == 0) { // If we have finished the requested recursions
    // return the last triangle in the chain
    return nextTriangle;
  }

  // Repeat for each vertex
  // Returning the results of our new triangle sierpinskied
  return generate_sierpinski_triangle(recursions, nextTriangle);
}

} // namespace Shapes

#pragma endregion Shape generation

} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
