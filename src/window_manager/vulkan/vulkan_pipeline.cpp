

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
    std::shared_ptr<VulkanDevice> vulkanDevice, std::shared_ptr<VulkanSwapChain> swapChain ) {
  Logger::log( "Constructing Vulkan pipeline...", Logger::DEBUG );

  Logger::log( "Loading shaders from: " + get_shader_path(), Logger::INFO );

  // Read shader file
  std::vector<char> slagShaderCode = read_file( get_shader_path() + +"triangle_shader.slang.spv" );

  // Create shader module
  vk::raii::ShaderModule shaderModule = create_shader_module( slagShaderCode, vulkanDevice );

  // Create shader stage info
  vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain" };
  vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
      .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };

  // Create shader stages
  vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  // Vertex input state info
  vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

  // Set to triangle topology
  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
      .topology = vk::PrimitiveTopology::eTriangleList };

  // Set viewport / scissor
  vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

  // Create rasterizer info
  vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False,
                                                       .rasterizerDiscardEnable = vk::False,
                                                       .polygonMode = vk::PolygonMode::eFill,
                                                       .cullMode = vk::CullModeFlagBits::eBack,
                                                       .frontFace = vk::FrontFace::eClockwise,
                                                       .depthBiasEnable = vk::False,
                                                       .lineWidth = 1.0f };

  // Multisampling info (currently disabled)
  vk::PipelineMultisampleStateCreateInfo multisampling{
      .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };

  // Color blending
  vk::PipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = vk::False,
      .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };

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
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0,
                                                   .pushConstantRangeCount = 0 };

  // Create pipeline
  std::shared_ptr<VulkanPipeline> pipeline = std::make_shared<VulkanPipeline>();

  // Create pipeline layout
  pipeline->pipelineLayout = vk::raii::PipelineLayout( vulkanDevice->device, pipelineLayoutInfo );

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
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = pipeline->pipelineLayout,
            .renderPass = nullptr },
          { .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapChain->swapChainSurfaceFormat.format } };

  // Create graphics pipeline
  pipeline->graphicsPipeline =
      vk::raii::Pipeline( vulkanDevice->device, nullptr,
                          pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>() );

  return pipeline;

  /// #################
  // ### Deprecated ###
  /// #################

  // auto vertShaderCode = read_file( get_shader_path() + "texture.vert.spv" );
  // auto fragShaderCode = read_file( get_shader_path() + +"texture.frag.spv" );

  // VkShaderModule vertShaderModule = create_shader_module( vertShaderCode, vulkanDevice->device );
  // VkShaderModule fragShaderModule = create_shader_module( fragShaderCode, vulkanDevice->device );

  // VkPipelineShaderStageCreateInfo vertShaderStageInfo =
  //     Initializer::vert_shader_stage_info( vertShaderModule );

  // VkPipelineShaderStageCreateInfo fragShaderStageInfo =
  //     Initializer::frag_shader_stage_info( fragShaderModule );

  // VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  // // ### vertex input ###
  // //   VkPipelineVertexInputStateCreateInfo vertexInputInfo =
  // //       Initializer::vertex_input_info();

  // VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  // vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  // auto bindingDescription = Vertex::get_binding_description();
  // auto attributeDescriptions = Vertex::get_attribute_descriptions();

  // vertexInputInfo.vertexBindingDescriptionCount = 1;
  // vertexInputInfo.vertexAttributeDescriptionCount =
  //     static_cast<uint32_t>( attributeDescriptions.size() );
  // vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  // vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  // // ### input assembly ###
  // VkPipelineInputAssemblyStateCreateInfo inputAssembly = Initializer::input_assembly();

  // // ### viewport & scissor ###
  // VkViewport viewport =
  //     Initializer::viewport( (float)swapChain->extent.height, (float)swapChain->extent.width );

  // VkRect2D scissor = Initializer::scissor( swapChain->extent );

  // VkPipelineViewportStateCreateInfo viewportState{};
  // viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  // viewportState.viewportCount = 1;
  // viewportState.pViewports = &viewport;
  // viewportState.scissorCount = 1;
  // viewportState.pScissors = &scissor;

  // // ### rasterization ###
  // VkPipelineRasterizationStateCreateInfo rasterizer = Initializer::rasterizer();

  // // ### multi-sampling ###
  // VkPipelineMultisampleStateCreateInfo multisampling =
  //     Initializer::multisampling( vulkanDevice->msaaSamples );

  // // ### color blending ###
  // VkPipelineColorBlendAttachmentState colorBlendAttachment =
  // Initializer::color_blend_attachment();

  // VkPipelineColorBlendStateCreateInfo colorBlending{};
  // colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  // colorBlending.logicOpEnable = VK_FALSE;
  // colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
  // colorBlending.attachmentCount = 1;
  // colorBlending.pAttachments = &colorBlendAttachment;
  // colorBlending.blendConstants[0] = 0.0f;  // Optional
  // colorBlending.blendConstants[1] = 0.0f;  // Optional
  // colorBlending.blendConstants[2] = 0.0f;  // Optional
  // colorBlending.blendConstants[3] = 0.0f;  // Optional

  // // ### depth & stencil ###
  // VkPipelineDepthStencilStateCreateInfo depthStencil{};
  // depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  // depthStencil.depthTestEnable = VK_TRUE;
  // depthStencil.depthWriteEnable = VK_TRUE;
  // depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  // depthStencil.depthBoundsTestEnable = VK_FALSE;
  // depthStencil.minDepthBounds = 0.0f;  // Optional
  // depthStencil.maxDepthBounds = 1.0f;  // Optional
  // depthStencil.stencilTestEnable = VK_FALSE;
  // depthStencil.front = {};  // Optional
  // depthStencil.back = {};   // Optional

  // // ### dynamic state ###
  // std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
  //                                               VK_DYNAMIC_STATE_SCISSOR };

  // VkPipelineDynamicStateCreateInfo dynamicState{};
  // dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  // dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
  // dynamicState.pDynamicStates = dynamicStates.data();

  // // ### pipeline layout ###

  // VulkanPipeline *pipeline = new VulkanPipeline();
  // VkPipelineLayoutCreateInfo pipelineLayoutInfo =
  //     Initializer::pipeline_layout_info( descriptorSetLayout );

  // if ( vkCreatePipelineLayout( vulkanDevice->device, &pipelineLayoutInfo, nullptr,
  //                              &pipeline->pipelineLayout ) != VK_SUCCESS ) {
  //   throw std::runtime_error( "failed to create pipeline layout!" );
  // }

  // VkGraphicsPipelineCreateInfo pipelineInfo{};
  // pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  // pipelineInfo.stageCount = 2;
  // pipelineInfo.pStages = shaderStages;
  // pipelineInfo.pVertexInputState = &vertexInputInfo;
  // pipelineInfo.pInputAssemblyState = &inputAssembly;
  // pipelineInfo.pViewportState = &viewportState;
  // pipelineInfo.pRasterizationState = &rasterizer;
  // pipelineInfo.pMultisampleState = &multisampling;
  // pipelineInfo.pDepthStencilState = &depthStencil;  // Optional
  // pipelineInfo.pColorBlendState = &colorBlending;
  // pipelineInfo.pDynamicState = &dynamicState;
  // pipelineInfo.layout = pipeline->pipelineLayout;
  // pipelineInfo.renderPass = swapChain->renderPass;
  // pipelineInfo.subpass = 0;
  // pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  // pipelineInfo.basePipelineIndex = -1;               // Optional

  // if ( vkCreateGraphicsPipelines( vulkanDevice->device, VK_NULL_HANDLE, 1, &pipelineInfo,
  // nullptr,
  //                                 &pipeline->graphicsPipeline ) != VK_SUCCESS ) {
  //   Logger::log( "Failed to create graphics pipeline!", Logger::CRITICAL );
  // }

  // // ### destroy ###
  // vkDestroyShaderModule( vulkanDevice->device, fragShaderModule, nullptr );
  // vkDestroyShaderModule( vulkanDevice->device, vertShaderModule, nullptr );

  // return pipeline;
}

// void destroy_graphics_pipeline( VkDevice device, VulkanPipeline *pipeline ) {
//   vkDestroyPipeline( device, pipeline->graphicsPipeline, nullptr );
//   vkDestroyPipelineLayout( device, pipeline->pipelineLayout, nullptr );
// }

[[nodiscard]] vk::raii::ShaderModule create_shader_module(
    const std::vector<char> &code, std::shared_ptr<VulkanDevice> vulkanDevice ) {
  // VkShaderModuleCreateInfo createInfo = Initializer::shader_module_create_info( code );
  vk::ShaderModuleCreateInfo createInfo{
      .codeSize = code.size() * sizeof( char ),
      .pCode = reinterpret_cast<const uint32_t *>( code.data() ) };

  // VkShaderModule shaderModule;
  vk::raii::ShaderModule shaderModule{ vulkanDevice->device, createInfo };

  // if ( vkCreateShaderModule( vulkanDevice, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
  // {
  //   Logger::log( "Failed to create shader module!", Logger::CRITICAL );
  // }

  return shaderModule;
}
}  // namespace Vulkan
}  // namespace Windows
}  // namespace Core
}  // namespace Thumpy
