#include "helios_renderer.hpp"
#include "helios_device.hpp"
#include "helios_swap_chain.hpp"
#include "helios_window.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_FORCE_ZERO_TO_ONE
#include <vulkan/vulkan_core.h>

// std
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace helios {

HeliosRenderer::HeliosRenderer(HeliosWindow &window, HeliosDevice &device)
    : heliosWindow{window}, heliosDevice{device} {
  recreateSwapChain();
  createCommandBuffers();
}

HeliosRenderer::~HeliosRenderer() { freeCommandBuffers(); }

void HeliosRenderer::recreateSwapChain() {
  auto extent = heliosWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = heliosWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(heliosDevice.device());

  if (heliosSwapChain == nullptr) {
    heliosSwapChain = std::make_unique<HeliosSwapChain>(heliosDevice, extent);
  } else {
    heliosSwapChain = std::make_unique<HeliosSwapChain>(
        heliosDevice, extent, std::move(heliosSwapChain));
    if (heliosSwapChain->imageCount() != commandBuffers.size()) {
      freeCommandBuffers();
      createCommandBuffers();
    }
  }
}

void HeliosRenderer::createCommandBuffers() {
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

void HeliosRenderer::freeCommandBuffers() {
  vkFreeCommandBuffers(heliosDevice.device(), heliosDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
}

VkCommandBuffer HeliosRenderer::beginFrame() {
  assert(!isFrameStarted &&
         "can't call beginFrame with frame already in progress");
  ;
  auto result = heliosSwapChain->acquireNextImage(&currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::runtime_error("failed to acquire next swap chain image");
  }

  isFrameStarted = true;

  auto commandBuffer = getCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    std::runtime_error("failed to begin recording command buffer");
  }

  return commandBuffer;
}

void HeliosRenderer::endFrame() {
  assert(isFrameStarted &&
         "can't call endFrame while frame is not in progress");
  auto commandBuffer = getCurrentCommandBuffer();
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    std::runtime_error("failed to record command buffer");
  }

  auto result =
      heliosSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      heliosWindow.wasWindowResized()) {
    heliosWindow.resetWindowResizedFlag();
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }

  isFrameStarted = false;
}

void HeliosRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "can't call beginSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "can't begin render pass on a command buffer from a different frame");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = heliosSwapChain->getRenderPass();
  renderPassInfo.framebuffer =
      heliosSwapChain->getFrameBuffer(currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = heliosSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width =
      static_cast<float>(heliosSwapChain->getSwapChainExtent().width);
  viewport.height =
      static_cast<float>(heliosSwapChain->getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, heliosSwapChain->getSwapChainExtent()};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void HeliosRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
  assert(isFrameStarted &&
         "can't call endSwapChainRenderPass if frame is not in progress");
  assert(commandBuffer == getCurrentCommandBuffer() &&
         "can't end render pass on a command buffer from a different frame");

  vkCmdEndRenderPass(commandBuffer);
}
} // namespace helios
