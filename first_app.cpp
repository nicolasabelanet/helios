#include "first_app.hpp"
#include "helios_device.hpp"
#include "helios_model.hpp"
#include "helios_pipeline.hpp"
#include "helios_swap_chain.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace helios {

FirstApp::FirstApp() {
  loadModels();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(heliosDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!heliosWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(heliosDevice.device());
};

void FirstApp::loadModels() {
  std::vector<HeliosModel::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  heliosModel = std::make_unique<HeliosModel>(heliosDevice, vertices);
}

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
      heliosSwapChain->width(), heliosSwapChain->height());
  pipelineConfig.renderPass = heliosSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  heliosPipeline = std::make_unique<HeliosPipeline>(
      heliosDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv", pipelineConfig);
}

void FirstApp::recreateSwapChain() {
  auto extent = heliosWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = heliosWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(heliosDevice.device());

  if (heliosSwapChain != nullptr) {
    heliosSwapChain->destroySwapChain();
  }
  heliosSwapChain = std::make_unique<HeliosSwapChain>(heliosDevice, extent);
  createPipeline();
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(heliosSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};

  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = heliosDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(heliosDevice.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers");
  }
}

void FirstApp::recordCommandBuffer(int imageIndex) {

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
      VK_SUCCESS) {
    std::runtime_error("failed to begin recording command buffer");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = heliosSwapChain->getRenderPass();
  renderPassInfo.framebuffer = heliosSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = heliosSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[1].depthStencil = {1, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  heliosPipeline->bind(commandBuffers[imageIndex]);
  heliosModel->bind(commandBuffers[imageIndex]);
  heliosModel->draw(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);

  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    std::runtime_error("failed to record command buffer");
  }
}

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  auto result = heliosSwapChain->acquireNextImage(&imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::runtime_error("failed to acquire next swap chain image");
  }

  recordCommandBuffer(imageIndex);
  result = heliosSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                                 &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      heliosWindow.wasWindowResized()) {
    heliosWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }
}

} // namespace helios
