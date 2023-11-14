#include "first_app.hpp"
#include "helios_pipeline.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <memory>
#include <stdexcept>

namespace helios {

FirstApp::FirstApp() {
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(heliosDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!heliosWindow.shouldClose()) {
    glfwPollEvents();
  }
};

void FirstApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pSetLayouts = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  if (vkCreatePipelineLayout(heliosDevice.device(), &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void FirstApp::createPipeline() {
  auto pipelineConfig = HeliosPipeline::defaultPipelineConfigInfo(
      heliosSwapChain.width(), heliosSwapChain.height());
  pipelineConfig.renderPass = heliosSwapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  heliosPipeline = std::make_unique<HeliosPipeline>(
      heliosDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {}
void FirstApp::drawFrame() {}

} // namespace helios
