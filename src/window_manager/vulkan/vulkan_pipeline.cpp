

#include "vulkan_pipeline.hpp"

#include <fstream>
#include <ios>

#include "logger.hpp"
#include "logger_helper.hpp"
#include "vulkan_initializers.hpp"

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#include <filesystem>
#endif  // _WIN32

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

// Please move this to a better place
static std::vector<char> read_file( const std::string &filename ) {
  Logger::log( "opening file: " + filename, Logger::INFO );
  std::ifstream file( filename, std::ios::ate | std::ios::binary );

  if ( !file.is_open() ) {
    Logger::log( "failed to open file: " + filename, Logger::CRITICAL );
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer( fileSize );

  file.seekg( 0 );
  file.read( buffer.data(), fileSize );

  file.close();

  return buffer;
}

std::string get_exe_path() {
#ifdef __unix__
  char result[PATH_MAX];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  std::string path = std::string( result, ( count > 0 ) ? count : 0 );
  path = path.substr( 0, path.find_last_of( "\\/" ) );
  return path;
#elif _WIN32
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);

    std::wstring ws = std::wstring(path);
    size_t len = wcstombs(nullptr, ws.c_str(), 0) + 1;
    char* buffer = new char[len];

    wcstombs(buffer, ws.c_str(), len);
    std::string string = std::string(buffer);
    string = string.substr(0, string.find_last_of("\\/"));
    return string;
#endif
  Logger::log( "Error determining os for filepath.", Logger::CRITICAL );
  return "";
}

VulkanPipeline create_graphics_pipeline(
    VulkanSwapChain *swapChain, VkDevice vulkanDevice,
    VkDescriptorSetLayout descriptorSetLayout ) {
  Logger::log( "Loading shaders from: " + get_exe_path(), Logger::INFO );
  auto vertShaderCode =
      read_file( get_exe_path() + "/shaders/uniform_buffer.vert.spv" );
  auto fragShaderCode = read_file( get_exe_path() + +"/shaders/vert.frag.spv" );

  VkShaderModule vertShaderModule =
      create_shader_module( vertShaderCode, vulkanDevice );
  VkShaderModule fragShaderModule =
      create_shader_module( fragShaderCode, vulkanDevice );

  VkPipelineShaderStageCreateInfo vertShaderStageInfo =
      Initializer::vert_shader_stage_info( vertShaderModule );

  VkPipelineShaderStageCreateInfo fragShaderStageInfo =
      Initializer::frag_shader_stage_info( fragShaderModule );

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
                                                     fragShaderStageInfo };

  // ### vertex input ###
  //   VkPipelineVertexInputStateCreateInfo vertexInputInfo =
  //       Initializer::vertex_input_info();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  auto bindingDescription = Vertex::get_binding_description();
  auto attributeDescriptions = Vertex::get_attribute_descriptions();

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>( attributeDescriptions.size() );
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // ### input assembly ###
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      Initializer::input_assembly();

  // ### viewport & scissor ###
  VkViewport viewport = Initializer::viewport( (float)swapChain->extent.height,
                                               (float)swapChain->extent.width );

  VkRect2D scissor = Initializer::scissor( swapChain->extent );

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  // ### rasterization ###
  VkPipelineRasterizationStateCreateInfo rasterizer = Initializer::rasterizer();

  // ### multi-sampling ###
  VkPipelineMultisampleStateCreateInfo multisampling =
      Initializer::multisampling();

  // ### color blending ###
  VkPipelineColorBlendAttachmentState colorBlendAttachment =
      Initializer::color_blend_attachment();

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;  // Optional
  colorBlending.blendConstants[1] = 0.0f;  // Optional
  colorBlending.blendConstants[2] = 0.0f;  // Optional
  colorBlending.blendConstants[3] = 0.0f;  // Optional

  // ### dynamic state ###
  std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount =
      static_cast<uint32_t>( dynamicStates.size() );
  dynamicState.pDynamicStates = dynamicStates.data();

  // ### pipeline layout ###

  VulkanPipeline pipeline;
  VkPipelineLayoutCreateInfo pipelineLayoutInfo =
      Initializer::pipeline_layout_info( descriptorSetLayout );

  if ( vkCreatePipelineLayout( vulkanDevice, &pipelineLayoutInfo, nullptr,
                               &pipeline.pipelineLayout ) != VK_SUCCESS ) {
    throw std::runtime_error( "failed to create pipeline layout!" );
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr;  // Optional
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline.pipelineLayout;
  pipelineInfo.renderPass = swapChain->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipelineInfo.basePipelineIndex = -1;               // Optional

  if ( vkCreateGraphicsPipelines( vulkanDevice, VK_NULL_HANDLE, 1,
                                  &pipelineInfo, nullptr,
                                  &pipeline.graphicsPipeline ) != VK_SUCCESS ) {
    Logger::log( "Failed to create graphics pipeline!", Logger::CRITICAL );
  }

  // ### destroy ###
  vkDestroyShaderModule( vulkanDevice, fragShaderModule, nullptr );
  vkDestroyShaderModule( vulkanDevice, vertShaderModule, nullptr );

  return pipeline;
}

void destroy_graphics_pipeline( VkDevice device, VulkanPipeline pipeline ) {
  vkDestroyPipeline( device, pipeline.graphicsPipeline, nullptr );
  vkDestroyPipelineLayout( device, pipeline.pipelineLayout, nullptr );
}

VkShaderModule create_shader_module( const std::vector<char> &code,
                                     VkDevice vulkanDevice ) {
  VkShaderModuleCreateInfo createInfo =
      Initializer::shader_module_create_info( code );

  VkShaderModule shaderModule;
  if ( vkCreateShaderModule( vulkanDevice, &createInfo, nullptr,
                             &shaderModule ) != VK_SUCCESS ) {
    Logger::log( "Failed to create shader module!", Logger::CRITICAL );
  }

  return shaderModule;
}
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
