#pragma once

#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <string>

namespace helios {

class HeliosWindow {

public:
  HeliosWindow(int w, int h, std::string name);
  ~HeliosWindow();

  HeliosWindow(const HeliosWindow &) = delete;
  HeliosWindow &operator=(const HeliosWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); };
  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  void initWindow();
  GLFWwindow *window;

  const int width;
  const int height;

  std::string window_name;
};

} // namespace helios
