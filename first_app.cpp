#include "first_app.hpp"
#include "helios_device.hpp"
#include "helios_game_object.hpp"
#include "helios_model.hpp"
#include "helios_pipeline.hpp"
#include "helios_swap_chain.hpp"
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_FORCE_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace helios {

struct SimplePushConstantData {
  glm::mat2 transform{1.0f};
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

FirstApp::FirstApp() {
  loadGameObjects();
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

void FirstApp::loadGameObjects() {
  std::vector<HeliosModel::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  auto heliosModel = std::make_shared<HeliosModel>(heliosDevice, vertices);

  auto triangle = HeliosGameObject::createGameObject();
  triangle.model = heliosModel;
  triangle.color = {0.1f, 0.8f, 0.1f};
  triangle.transform2d.translation.x = .2f;
  triangle.transform2d.scale = {2.0f, 0.5f};
  triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

  gameObjects.push_back(std::move(triangle));
}

void FirstApp::createPipelineLayout() {

  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(SimplePushConstantData);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pSetLayouts = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  if (vkCreatePipelineLayout(heliosDevice.device(), &pipelineLayoutInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout");
  }
}

void FirstApp::createPipeline() {

  assert(heliosSwapChain != nullptr &&
         "cannot create pipeline before swap chain");
  assert(pipelineLayout != nullptr &&
         "cannot create pipeline before pipeline layout");

  PipelineConfigInfo pipelineConfig{};
  HeliosPipeline::defaultPipelineConfigInfo(pipelineConfig);
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

void FirstApp::freeCommandBuffers() {
  vkFreeCommandBuffers(heliosDevice.device(), heliosDevice.getCommandPool(),
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  commandBuffers.clear();
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
  clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
  clearValues[1].depthStencil = {1, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();
  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
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
  vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
  vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

  renderGameObjects(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);

  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    std::runtime_error("failed to record command buffer");
  }
}

void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
  heliosPipeline->bind(commandBuffer);
  for (auto &obj : gameObjects) {

    for (int j = 0; j < 4; j++) {
      obj.transform2d.rotation =
          glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
      SimplePushConstantData push{};
      push.offset = obj.transform2d.translation;
      push.color = obj.color;
      push.transform = obj.transform2d.mat2();

      vkCmdPushConstants(commandBuffer, pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT |
                             VK_SHADER_STAGE_FRAGMENT_BIT,
                         0, sizeof(SimplePushConstantData), &push);
      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
    }
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
