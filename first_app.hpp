#pragma once
#include "helios_device.hpp"
#include "helios_game_object.hpp"
#include "helios_renderer.hpp"
#include "helios_window.hpp"

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
  void loadGameObjects();

  HeliosWindow heliosWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  HeliosDevice heliosDevice{heliosWindow};
  HeliosRenderer heliosRenderer{heliosWindow, heliosDevice};

  std::vector<HeliosGameObject> gameObjects;
};

} // namespace helios
