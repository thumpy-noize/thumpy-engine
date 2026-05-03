

/**
 * @file vulkan_pipeline.cpp
 * @author Thumpy (◕‿◕✿)
 * @brief Vulkan pipeline cpp file
 * @version 0.1
 * @date 2024-12-13
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "vulkan_pipeline.hpp"

#include <fstream>
#include <ios>
#include <string>

#include "logger.hpp"
#include "vulkan_helper.hpp"
#include "vulkan_image.hpp"
#include "vulkan_initializers.hpp"

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

// TODO: Please move this to a better place,
// Reading / Writing files will be a vital part of the game engine
static std::vector<char> read_file( const std::string &filename ) {
  Logger::log( "Opening file: " + filename, Logger::INFO );
  std::ifstream file( filename, std::ios::ate | std::ios::binary );

  if ( !file.is_open() ) {
    Logger::log( "Failed to open file: " + filename, Logger::ERROR_LOG );
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer( fileSize );

  file.seekg( 0 );
  file.read( buffer.data(), fileSize );

  file.close();

  return buffer;
}

std::shared_ptr<VulkanPipeline> create_graphics_pipeline(
    std::shared_ptr<VulkanDevice> vulkanDevice, std::shared_ptr<VulkanSwapChain> swapChain,
    vk::raii::DescriptorSetLayout &descriptorSetLayout ) {
  Logger::log( "Constructing Vulkan pipeline...", Logger::DEBUG );

  // Read shader file
  std::vector<char> slagShaderCode = read_file( get_shader_path() + "texture_shader.slang.spv" );

  // Create shader module
  vk::raii::ShaderModule shaderModule = create_shader_module( slagShaderCode, vulkanDevice );

  // Create shader stage info
  vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };

  vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };

  // Create shader stages
  vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  // Get vertex bindings
  auto bindingDescription = Vertex::get_binding_description();
  // Get vertex attributes
  auto attributeDescriptions = Vertex::get_attribute_descriptions();

  // Vertex input state info
  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &bindingDescription,
      .vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescriptions.size() ),
      .pVertexAttributeDescriptions = attributeDescriptions.data() };

  // Set to triangle topology
  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
      .topology = vk::PrimitiveTopology::eTriangleList };

  // Set viewport / scissor
  vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

  // Create rasterizer info
  vk::PipelineRasterizationStateCreateInfo rasterizer = Initializer::rasterizer();

  // Multisampling info
  vk::PipelineMultisampleStateCreateInfo multisampling =
      Initializer::multisampling( vulkanDevice->msaaSamples );

  // Depth stencil info
  vk::PipelineDepthStencilStateCreateInfo depthStencil{ .depthTestEnable = vk::True,
                                                        .depthWriteEnable = vk::True,
                                                        .depthCompareOp = vk::CompareOp::eLess,
                                                        .depthBoundsTestEnable = vk::False,
                                                        .stencilTestEnable = vk::False };

  // Color blending
  vk::PipelineColorBlendAttachmentState colorBlendAttachment =
      Initializer::color_blend_attachment();

  vk::PipelineColorBlendStateCreateInfo colorBlending{ .logicOpEnable = vk::False,
                                                       .logicOp = vk::LogicOp::eCopy,
                                                       .attachmentCount = 1,
                                                       .pAttachments = &colorBlendAttachment };

  // Dynamic States
  std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport,
                                                  vk::DynamicState::eScissor };
  vk::PipelineDynamicStateCreateInfo dynamicState{
      .dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() ),
      .pDynamicStates = dynamicStates.data() };

  // Pipeline layout info
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo =
      Initializer::pipeline_layout_info( descriptorSetLayout );

  // Create pipeline
  std::shared_ptr<VulkanPipeline> pipeline = std::make_shared<VulkanPipeline>();

  // Create pipeline layout
  pipeline->pipelineLayout = vk::raii::PipelineLayout( vulkanDevice->device, pipelineLayoutInfo );

  // Find depth format
  vk::Format depthFormat = Image::find_depth_format( vulkanDevice->physicalDevice );

  // Create graphics pipeline info
  vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo>
      pipelineCreateInfoChain = {
          { .stageCount = 2,
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = pipeline->pipelineLayout,
            .renderPass = nullptr },
          { .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapChain->swapChainSurfaceFormat.format,
            .depthAttachmentFormat = depthFormat } };

  Logger::log( "Constructing graphics pipeline...", Logger::DEBUG );

  // Create graphics pipeline
  pipeline->graphicsPipeline =
      vk::raii::Pipeline( vulkanDevice->device, nullptr,
                          pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>() );

  return pipeline;
}

[[nodiscard]] vk::raii::ShaderModule create_shader_module(
    const std::vector<char> &code, std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // Create shader module info
  vk::ShaderModuleCreateInfo createInfo = Initializer::shader_module_create_info( code );

  // Create shader module
  vk::raii::ShaderModule shaderModule{ vulkanDevice->device, createInfo };

  return shaderModule;
}
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
