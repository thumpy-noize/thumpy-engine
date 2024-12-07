/**
 * @file vulkan_helper.hpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan helper functions
 * @version 0.1
 * @date 2024-11-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

#include "logger.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool is_complete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription get_binding_description() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof( Vertex );
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof( Vertex, pos );

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof( Vertex, color );

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof( Vertex, texCoord );

    return attributeDescriptions;
  }

  static Vertex mid( Vertex a, Vertex b ) {
    Vertex vert;
    vert.pos = ( a.pos + b.pos ) / glm::vec3( 2 );
    vert.color = ( a.color + b.color ) / glm::vec3( 2 );
    return vert;
  }
};

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

struct VulkanImage {
  VkImage image;
  VkDeviceMemory imageMemory;
  VkImageView imageView;
  VkSampler sampler;

  void destroy( VkDevice device, bool destroySampler = true ) {
    if ( destroySampler ) vkDestroySampler( device, sampler, nullptr );
    vkDestroyImageView( device, imageView, nullptr );
    vkDestroyImage( device, image, nullptr );
    vkFreeMemory( device, imageMemory, nullptr );
  }
};

class VulkanNotCompatible : public std::exception {
 private:
  std::string message_;

 public:
  // Constructor accepts a const char* that is used to set
  // the exception message
  VulkanNotCompatible( const char *msg ) : message_( msg ) {
    Logger::log( "Failed to find GPUs with Vulkan support!", Logger::ERROR );
  }

  // Override the what() method to return our message
  const char *what() const throw() { return message_.c_str(); }
};

bool check_validation_layer_support();

std::vector<const char *> get_required_extensions();

uint32_t find_memory_type( VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                           VkMemoryPropertyFlags properties );

namespace Shapes {

std::vector<Vertex> generate_sierpinski_triangle( uint32_t recursions,
                                                  std::vector<Vertex> startingTriangle );
}

#pragma region Asset loading

struct Texture {
  unsigned char *pixels;
  int width, height, channels;
  VkDeviceSize imageSize;
};
Texture *load_texture( std::string filePath );
void free_texture( Texture *texture );

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
};

Mesh *load_mesh( std::string filePath );

#pragma endregion Asset loading

#pragma region Paths
std::string get_exe_path();
std::string get_assets_path();
std::string get_shader_path();
std::string get_texture_path();
std::string get_model_path();
#pragma endregion

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
