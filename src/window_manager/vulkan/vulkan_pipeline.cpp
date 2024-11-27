

#include "vulkan_pipeline.hpp"
#include "logger.hpp"
#include "vulkan_initializers.hpp"
#include <fstream>
#include <ios>
#include <stdexcept>

namespace Thumpy {
namespace Core {
namespace Windows {
namespace Vulkan {

// Please fix this and move it to a better place
static std::vector<char> read_file(const std::string &filename) {
  Logger::log("opening file: " + filename, Logger::INFO);
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + filename);
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

VulkanPipeline create_graphics_pipeline(VulkanSwapChain *swapChain,
                                        VkDevice vulkanDevice) {
  auto vertShaderCode = read_file("src/shaders/compiled/vert.spv");
  auto fragShaderCode = read_file("src/shaders/compiled/frag.spv");

  VkShaderModule vertShaderModule =
      create_shader_module(vertShaderCode, vulkanDevice);
  VkShaderModule fragShaderModule =
      create_shader_module(fragShaderCode, vulkanDevice);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo =
      Initializer::vert_shader_stage_info(vertShaderModule);

  VkPipelineShaderStageCreateInfo fragShaderStageInfo =
      Initializer::frag_shader_stage_info(fragShaderModule);

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // ### vertex input ###
  VkPipelineVertexInputStateCreateInfo vertexInputInfo =
      Initializer::vertex_input_info();

  // ### input assembly ###
  VkPipelineInputAssemblyStateCreateInfo inputAssembly =
      Initializer::input_assembly();

  // ### viewport & scissor ###
  VkViewport viewport = Initializer::viewport((float)swapChain->extent.height,
                                              (float)swapChain->extent.width);

  VkRect2D scissor = Initializer::scissor(swapChain->extent);

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
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional

  // ### dynamic state ###
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // ### pipeline layout ###

  VulkanPipeline pipeline;
  VkPipelineLayoutCreateInfo pipelineLayoutInfo =
      Initializer::pipeline_layout_info();

  if (vkCreatePipelineLayout(vulkanDevice, &pipelineLayoutInfo, nullptr,
                             &pipeline.pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
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
  pipelineInfo.pDepthStencilState = nullptr; // Optional
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.layout = pipeline.pipelineLayout;
  pipelineInfo.renderPass = swapChain->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1;              // Optional

  if (vkCreateGraphicsPipelines(vulkanDevice, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr,
                                &pipeline.graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  // ### destroy ###
  vkDestroyShaderModule(vulkanDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(vulkanDevice, vertShaderModule, nullptr);

  return pipeline;
}

void destroy_graphics_pipeline(VkDevice device, VulkanPipeline pipeline) {
  vkDestroyPipeline(device, pipeline.graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, pipeline.pipelineLayout, nullptr);
}

VkShaderModule create_shader_module(const std::vector<char> &code,
                                    VkDevice vulkanDevice) {
  VkShaderModuleCreateInfo createInfo =
      Initializer::shader_module_create_info(code);

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(vulkanDevice, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}
} // namespace Vulkan
} // namespace Windows
} // namespace Core
} // namespace Thumpy
