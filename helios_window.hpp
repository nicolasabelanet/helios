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
  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  };
  bool wasWindowResized() { return frameBufferResized; };
  void resetWindowResizedFlag() { frameBufferResized = false; };
  GLFWwindow *getGLFWwindow() const { return window; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);
  void initWindow();
  GLFWwindow *window;

  int width;
  int height;
  bool frameBufferResized = false;

  std::string windowName;
};

} // namespace helios
