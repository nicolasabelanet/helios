#pragma once
#include "helios_device.hpp"
#include "helios_model.hpp"
#include "helios_pipeline.hpp"
#include "helios_swap_chain.hpp"
#include "helios_window.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <memory>
#include <vector>

namespace helios {

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

private:
  void loadModels();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void freeCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);

  HeliosWindow heliosWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  HeliosDevice heliosDevice{heliosWindow};
  std::unique_ptr<HeliosSwapChain> heliosSwapChain;
  std::unique_ptr<HeliosPipeline> heliosPipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<HeliosModel> heliosModel;
};

} // namespace helios
