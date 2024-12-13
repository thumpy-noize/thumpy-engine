
/**
 * @file vulkan_helper.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan helper cpp file
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */
// #define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <cstring>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan_helper.hpp"

#ifdef __unix__
#include <unistd.h>

#elif _WIN32
#include <windows.h>

#include <filesystem>
#endif  // _WIN32

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

bool check_validation_layer_support() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

  std::vector<VkLayerProperties> availableLayers( layerCount );
  vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

  for ( const char *layerName : validationLayers ) {
    bool layerFound = false;

    for ( const auto &layerProperties : availableLayers ) {
      if ( strcmp( layerName, layerProperties.layerName ) == 0 ) {
        layerFound = true;
        break;
      }
    }

    if ( !layerFound ) {
      return false;
    }
  }

  return true;
}

std::vector<const char *> get_required_extensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

  std::vector<const char *> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

  if ( enableValidationLayers ) {
    extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
  }

  return extensions;
}

uint32_t find_memory_type( VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                           VkMemoryPropertyFlags properties ) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties );

  for ( uint32_t i = 0; i < memProperties.memoryTypeCount; i++ ) {
    if ( ( typeFilter & ( 1 << i ) ) &&
         ( memProperties.memoryTypes[i].propertyFlags & properties ) == properties ) {
      return i;
    }
  }

  Logger::log( "failed to find suitable memory type!", Logger::CRITICAL );
  return 0;
}

VkSampleCountFlagBits get_max_usable_sample_count( VkPhysicalDevice physicalDevice ) {
  VkPhysicalDeviceProperties physicalDeviceProperties;
  vkGetPhysicalDeviceProperties( physicalDevice, &physicalDeviceProperties );

  VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                              physicalDeviceProperties.limits.framebufferDepthSampleCounts;
  if ( counts & VK_SAMPLE_COUNT_64_BIT ) {
    Logger::log( "Sample count: 64 bits", Logger::DEBUG );
    return VK_SAMPLE_COUNT_64_BIT;
  }
  if ( counts & VK_SAMPLE_COUNT_32_BIT ) {
    Logger::log( "Sample count: 32 bits", Logger::DEBUG );
    return VK_SAMPLE_COUNT_32_BIT;
  }
  if ( counts & VK_SAMPLE_COUNT_16_BIT ) {
    Logger::log( "Sample count: 16 bits", Logger::DEBUG );
    return VK_SAMPLE_COUNT_16_BIT;
  }
  if ( counts & VK_SAMPLE_COUNT_8_BIT ) {
    Logger::log( "Sample count: 8 bits", Logger::INFO );
    return VK_SAMPLE_COUNT_8_BIT;
  }
  if ( counts & VK_SAMPLE_COUNT_4_BIT ) {
    Logger::log( "Sample count: 4 bits", Logger::INFO );
    return VK_SAMPLE_COUNT_4_BIT;
  }
  if ( counts & VK_SAMPLE_COUNT_2_BIT ) {
    Logger::log( "Sample count: 2 bits", Logger::INFO );
    return VK_SAMPLE_COUNT_2_BIT;
  }

  Logger::log( "Sample count: 1 bits", Logger::INFO );
  return VK_SAMPLE_COUNT_1_BIT;
}

#pragma region Shape generation

namespace Shapes {

Mesh *generate_triangle() {
  Mesh *mesh = new Mesh();
  mesh->vertices = { { { 0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
                     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
                     { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } } };
  mesh->indices = { 0, 1, 2 };
  return mesh;

}  // namespace Shapes

Mesh *generate_square() {
  Mesh *mesh = new Mesh();
  mesh->vertices = { { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
                     { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
                     { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
                     { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } } };
  mesh->indices = { 0, 1, 2, 2, 3, 0 };

  return mesh;
}

/**
 * @brief Creates a sierpinski triangle from list of triangles
 * shout out to jacob keller for the guidance -
 * (https://www.vfxkeeler.com/post/vulkan-sierpinski-triangle)
 * @param recursions number of times to fractal
 * @param startingTriangle starting triangles
 * @return std::vector<Vertex> sierpinski triangles
 */
Mesh *generate_sierpinski_triangle( Mesh *startingMesh, uint32_t recursions ) {
  Logger::log( "Generating sierpinski - recursions left: " + std::to_string( recursions ) + "",
               Logger::INFO );

  /**
   * For each index
   * make a triangle out of mid points
   * add vertices to new map if they don't already exist in that map
   * add indexes ass you check / add verts
   */

  // std::vector<Vertex> nextTriangle;
  Mesh *nextMesh = new Mesh();
  nextMesh->indices = startingMesh->indices;
  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  // Create a new triangle for each vertex
  for ( int i = 0; i < startingMesh->indices.size(); i++ ) {
    int vertMod = i % 3;  // Vertex position in triangle
    int nextIndex;
    int lastIndex;
    Vertex lastVertex;
    Vertex nextVertex;
    switch ( vertMod ) {
      case 0:  // if A get B & C
        nextIndex = i + 1;
        lastIndex = i + 2;
        break;
      case 1:  // if B get C & A
        nextIndex = i + 1;
        lastIndex = i - 1;
        break;
      case 2:  // if C get B & A
        nextIndex = i - 2;
        lastIndex = i - 1;
        break;
      default:
        nextIndex = i;
        lastIndex = i;
    }

    // get midpoint between this vertex & next index
    nextVertex = Vertex::mid( startingMesh->vertices[startingMesh->indices[nextIndex]],
                              startingMesh->vertices[startingMesh->indices[i]] );
    // get midpoint between this vertex & last index
    lastVertex = Vertex::mid( startingMesh->vertices[startingMesh->indices[lastIndex]],
                              startingMesh->vertices[startingMesh->indices[i]] );

    nextMesh->vertices.push_back( nextVertex );  // midpoint
    nextMesh->vertices.push_back( lastVertex );  // midpoint
    nextMesh->vertices.push_back(
        startingMesh->vertices[startingMesh->indices[i]] );  // this vertex
  }

  Mesh *finalMesh = new Mesh();
  for ( int i = 0; i < nextMesh->vertices.size(); i++ ) {
    if ( uniqueVertices.count( nextMesh->vertices[i] ) == 0 ) {
      uniqueVertices[nextMesh->vertices[i]] = static_cast<uint32_t>( finalMesh->vertices.size() );
      finalMesh->vertices.push_back( nextMesh->vertices[i] );
    }

    finalMesh->indices.push_back( uniqueVertices[nextMesh->vertices[i]] );
  }

  recursions--;
  if ( recursions == 0 ) {  // If we have finished the requested recursions
                            // return the last triangle in the chain
    return finalMesh;
  }

  // Repeat for each vertex
  // Returning the results of our new triangle sierpinskied
  return generate_sierpinski_triangle( finalMesh, recursions );
}

}  // namespace Shapes

#pragma endregion Shape generation

#pragma region Asset loading

Texture *load_texture( std::string filePath ) {
  Logger::log( "Loading texture: " + get_texture_path() + filePath, Logger::DEBUG );
  Texture *texture = new Texture();

  texture->pixels =
      stbi_load( std::string( get_texture_path() + filePath ).c_str(), &texture->width,
                 &texture->height, &texture->channels, STBI_rgb_alpha );
  texture->imageSize = texture->width * texture->height * 4;

  if ( !texture->pixels ) {
    Logger::log( "Failed to load texture image!", Logger::ERROR_LOG );
  }

  return texture;
}

void free_texture( Texture *texture ) { stbi_image_free( texture->pixels ); }

Mesh *load_mesh( std::string filePath ) {
  std::string modelPath = get_model_path() + filePath;

  Logger::log( "Loading model: " + modelPath, Logger::DEBUG );

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  if ( !tinyobj::LoadObj( &attrib, &shapes, &materials, &err, modelPath.c_str() ) ) {
    Logger::log( err, Logger::ERROR_LOG );
  }

  Mesh *mesh = new Mesh();
  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for ( const auto &shape : shapes ) {
    for ( const auto &index : shape.mesh.indices ) {
      Vertex vertex{};

      vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
                     attrib.vertices[3 * index.vertex_index + 1],
                     attrib.vertices[3 * index.vertex_index + 2] };

      vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
                          1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

      vertex.color = { 1.0f, 1.0f, 1.0f };

      if ( uniqueVertices.count( vertex ) == 0 ) {
        uniqueVertices[vertex] = static_cast<uint32_t>( mesh->vertices.size() );
        mesh->vertices.push_back( vertex );
      }

      mesh->indices.push_back( uniqueVertices[vertex] );
    }
  }
  return mesh;
}

#pragma endregion Asset loading

#pragma region Paths

std::string exePath_;
std::string get_exe_path() {
  if ( exePath_.empty() ) {
#ifdef __unix__
    char result[PATH_MAX];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    exePath_ = std::string( result, ( count > 0 ) ? count : 0 );
    exePath_ = exePath_.substr( 0, exePath_.find_last_of( "\\/" ) );
    return exePath_;
#elif _WIN32
    wchar_t modulePath[MAX_PATH] = { 0 };
    GetModuleFileNameW( NULL, modulePath, MAX_PATH );

    std::wstring ws = std::wstring( modulePath );
    size_t len = wcstombs( nullptr, ws.c_str(), 0 ) + 1;
    char *buffer = new char[len];

    wcstombs( buffer, ws.c_str(), len );
    exePath_ = std::string( buffer );
    exePath_ = exePath_.substr( 0, exePath_.find_last_of( "\\/" ) );
    return exePath_;
#endif
    Logger::log( "Error determining os for filepath.", Logger::CRITICAL );
    return "";
  }
  return exePath_;
}

std::string assetsPath_;
std::string get_assets_path() {
  if ( assetsPath_.empty() ) {
    assetsPath_ = get_exe_path() + "/assets/";
  }
  return assetsPath_;
}

std::string shaderPath_;
std::string get_shader_path() {
  if ( shaderPath_.empty() ) {
    shaderPath_ = get_assets_path() + "shaders/";
  }
  return shaderPath_;
}

std::string texturePath_;
std::string get_texture_path() {
  if ( texturePath_.empty() ) {
    texturePath_ = get_assets_path() + "textures/";
  }
  return texturePath_;
}

std::string modelPath_;
std::string get_model_path() {
  if ( modelPath_.empty() ) {
    modelPath_ = get_assets_path() + "models/";
  }
  return modelPath_;
}

}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
