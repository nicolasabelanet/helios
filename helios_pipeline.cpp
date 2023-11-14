#include "helios_pipeline.hpp"
#include "vulkan/vulkan_core.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace helios {

HeliosPipeline::HeliosPipeline(HeliosDevice &device,
                               const std::string &vertFilepath,
                               const std::string &fragFilepath,
                               const PipelineConfigInfo &configInfo)
    : heliosDevice{device} {

  createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
}

std::vector<char> HeliosPipeline::readFile(const std::string &filepath) {

  std::ifstream file{filepath, std::ios::ate | std::ios::binary};

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + filepath);
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();
  return buffer;
}

void HeliosPipeline::createGraphicsPipeline(
    const std::string &vertFilepath, const std::string &fragFilepath,
    const PipelineConfigInfo &configInfo) {
  auto vertCode = readFile(vertFilepath);
  auto fragCode = readFile(fragFilepath);

  std::cout << "Vertex Shader code size " << vertCode.size() << '\n';
  std::cout << "Fragment Shader code size " << fragCode.size() << '\n';
}

void HeliosPipeline::createShaderModule(const std::vector<char> &code,
                                        VkShaderModule *shaderModule) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
  if (vkCreateShaderModule(heliosDevice.device(), &createInfo, nullptr,
                           shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
}

PipelineConfigInfo HeliosPipeline::defaultPipelineConfigInfo(uint32_t width,
                                                             uint32_t height) {

  PipelineConfigInfo configInfo{};
  return configInfo;
}

} // namespace helios
