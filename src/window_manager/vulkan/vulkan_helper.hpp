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
#define GLM_ENABLE_EXPERIMENTAL

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
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

struct Descriptors {
  VkDescriptorSetLayout setLayout;
  VkDescriptorPool pool;
  std::vector<VkDescriptorSet> sets;
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
    vert.texCoord = ( a.texCoord + b.texCoord ) / glm::vec2( 2 );
    return vert;
  }

  bool operator==( const Vertex &other ) const {
    return pos == other.pos && color == other.color && texCoord == other.texCoord;
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

  void destroy( VkDevice device ) {
    vkDestroyImageView( device, imageView, nullptr );
    vkDestroyImage( device, image, nullptr );
    vkFreeMemory( device, imageMemory, nullptr );
  }
};

struct VulkanTextureImage : VulkanImage {
  VkSampler sampler;
  uint32_t mipLevels;

  void destroy( VkDevice device ) {
    vkDestroySampler( device, sampler, nullptr );
    VulkanImage::destroy( device );
  }
};

bool check_validation_layer_support();

std::vector<const char *> get_required_extensions();

uint32_t find_memory_type( VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                           VkMemoryPropertyFlags properties );

VkSampleCountFlagBits get_max_usable_sample_count( VkPhysicalDevice physicalDevice );

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

#pragma region Shapes

namespace Shapes {

Mesh *generate_triangle();

Mesh *generate_square();

Mesh *generate_sierpinski_triangle( Mesh *startingMesh, uint32_t recursions );
}  // namespace Shapes

#pragma endregion Shapes

#pragma region Paths

std::string get_exe_path();
std::string get_assets_path();
std::string get_shader_path();
std::string get_texture_path();
std::string get_model_path();

#pragma endregion

#pragma region Exceptions

class VulkanNotCompatible : public std::exception {
 private:
  std::string message_;

 public:
  // Constructor accepts a const char* that is used to set
  // the exception message
  VulkanNotCompatible( const char *msg ) : message_( msg ) {
    Logger::log( "Failed to find GPUs with Vulkan support!", Logger::ERROR_LOG );
  }

  // Override the what() method to return our message
  const char *what() const throw() { return message_.c_str(); }
};

#pragma endregion

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy

namespace std {
template <>
struct hash<Thumpy::Core::Windows::Vulkan::Vertex> {
  size_t operator()( Thumpy::Core::Windows::Vulkan::Vertex const &vertex ) const {
    return ( ( hash<glm::vec3>()( vertex.pos ) ^ ( hash<glm::vec3>()( vertex.color ) << 1 ) ) >>
             1 ) ^
           ( hash<glm::vec2>()( vertex.texCoord ) << 1 );
  }
};
}  // namespace std