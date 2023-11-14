#pragma once

#include "helios_device.hpp"
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_FORCE_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace helios {

class HeliosModel {

public:
  struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription>
    getBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription>
    getAttributeDescriptions();
  };

  HeliosModel(HeliosDevice &device, const std::vector<Vertex> &vertices);
  ~HeliosModel();

  HeliosModel(const HeliosModel &) = delete;
  HeliosModel &operator=(const HeliosModel &) = delete;

  void bind(VkCommandBuffer commandBuffer);
  void draw(VkCommandBuffer commandBuffer);

private:
  void createVertexBuffers(const std::vector<Vertex> &vertices);
  HeliosDevice &heliosDevice;
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  uint32_t vertexCount;
};

} // namespace helios
