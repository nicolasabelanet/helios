#pragma once
#include "helios_camera.hpp"
#include "helios_device.hpp"
#include "helios_game_object.hpp"
#include "helios_pipeline.hpp"
#include "vulkan/vulkan_core.h"

// std
#include <memory>
#include <vector>

namespace helios {

class SimpleRenderSystem {
public:
  SimpleRenderSystem(HeliosDevice &device, VkRenderPass renderPass);
  ~SimpleRenderSystem();

  SimpleRenderSystem(const SimpleRenderSystem &) = delete;
  SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;
  void renderGameObjects(VkCommandBuffer commandBuffer,
                         std::vector<HeliosGameObject> &gameObjects,
                         const HeliosCamera &camera);

private:
  void createPipelineLayout();
  void createPipeline(VkRenderPass renderPass);

  HeliosDevice &heliosDevice;

  std::unique_ptr<HeliosPipeline> heliosPipeline;
  VkPipelineLayout pipelineLayout;
};

} // namespace helios
