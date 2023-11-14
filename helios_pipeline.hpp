#pragma once
#include "helios_device.hpp"
#include "vulkan/vulkan_core.h"

#include <string>
#include <vector>

namespace helios {

struct PipelineConfigInfo {
  PipelineConfigInfo(const PipelineConfigInfo &) = delete;
  PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class HeliosPipeline {

public:
  HeliosPipeline(HeliosDevice &device, const std::string &vertFilepath,
                 const std::string &fragFilepath,
                 const PipelineConfigInfo &configInfo);

  ~HeliosPipeline();

  HeliosPipeline(const HeliosPipeline &) = delete;
  HeliosPipeline &operator=(const HeliosPipeline &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

private:
  static std::vector<char> readFile(const std::string &filepath);

  void createGraphicsPipeline(const std::string &vertFilepath,
                              const std::string &fragFilepath,
                              const PipelineConfigInfo &configInfo);

  void createShaderModule(const std::vector<char> &code,
                          VkShaderModule *shaderModule);

  HeliosDevice &heliosDevice;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};

} // namespace helios
