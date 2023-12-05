#include "first_app.hpp"
#include "helios_camera.hpp"
#include "helios_device.hpp"
#include "helios_game_object.hpp"
#include "helios_model.hpp"
#include "helios_pipeline.hpp"
#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_FORCE_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan_core.h>

// std
#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace helios {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
  SimpleRenderSystem simpleRenderSystem{
      heliosDevice, heliosRenderer.getSwapChainRenderPass()};

  HeliosCamera camera{};
  camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f),
                       glm::vec3(0.0f, 0.0f, 2.5f));

  auto viewerObject = HeliosGameObject::createGameObject();
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();

  while (!heliosWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime -
                                                                   currentTime)
            .count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(heliosWindow.getGLFWwindow(), frameTime,
                                   viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation,
                      viewerObject.transform.rotation);

    float aspect = heliosRenderer.getAspectRatio();

    camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
    if (auto commandBuffer = heliosRenderer.beginFrame()) {
      heliosRenderer.beginSwapChainRenderPass(commandBuffer);
      simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
      heliosRenderer.endSwapChainRenderPass(commandBuffer);
      heliosRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(heliosDevice.device());
};

void FirstApp::loadGameObjects() {
  std::shared_ptr<HeliosModel> heliosModel =
      HeliosModel::createModelFromFile(heliosDevice, "models/smooth_vase.obj");
  auto gameObject = HeliosGameObject::createGameObject();
  gameObject.model = heliosModel;
  gameObject.transform.translation = {0.0f, 0.0f, 2.5f};
  gameObject.transform.scale = glm::vec3(3.0f);
  gameObjects.push_back(std::move(gameObject));
}

} // namespace helios
