#include "first_app.hpp"
#include "helios_device.hpp"
#include "helios_game_object.hpp"
#include "helios_model.hpp"
#include "helios_pipeline.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_FORCE_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan_core.h>

// std
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace helios {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{
      heliosDevice, heliosRenderer.getSwapChainRenderPass()};

  while (!heliosWindow.shouldClose()) {
    glfwPollEvents();
    if (auto commandBuffer = heliosRenderer.beginFrame()) {
      heliosRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
      heliosRenderer.endSwapChainRenderPass(commandBuffer);
      heliosRenderer.endFrame();
    }
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

} // namespace helios
