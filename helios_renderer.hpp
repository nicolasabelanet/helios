#pragma once
#include "helios_device.hpp"
#include "helios_swap_chain.hpp"
#include "helios_window.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace helios {

class HeliosRenderer {
public:
  HeliosRenderer(HeliosWindow &window, HeliosDevice &device);
  ~HeliosRenderer();

  HeliosRenderer(const HeliosRenderer &) = delete;
  HeliosRenderer &operator=(const HeliosRenderer &) = delete;

  VkRenderPass getSwapChainRenderPass() const {
    return heliosSwapChain->getRenderPass();
  }
  bool isFrameInProgress() const { return isFrameStarted; }

  VkCommandBuffer getCurrentCommandBuffer() const {
    assert(isFrameStarted &&
           "cannot get command buffer when frame not in progress");
    return commandBuffers[currentFrameIndex];
  }

  int getFrameIndex() const {
    assert(isFrameStarted &&
           "cannot get frame index when frame not in progress");
    return currentFrameIndex;
  }

  VkCommandBuffer beginFrame();
  void endFrame();
  void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
  void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
  void createCommandBuffers();
  void freeCommandBuffers();
  void recreateSwapChain();

  HeliosWindow &heliosWindow;
  HeliosDevice &heliosDevice;
  std::unique_ptr<HeliosSwapChain> heliosSwapChain;
  std::vector<VkCommandBuffer> commandBuffers;

  uint32_t currentImageIndex;
  int currentFrameIndex{0};

  bool isFrameStarted{false};
};

} // namespace helios
