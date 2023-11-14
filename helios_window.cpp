#include "helios_window.hpp"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace helios {

HeliosWindow::HeliosWindow(int w, int h, std::string name)
    : width{w}, height{h}, window_name{name} {
  initWindow();
}

HeliosWindow::~HeliosWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void HeliosWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
}

void HeliosWindow::createWindowSurface(VkInstance instance,
                                       VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface");
  }
}

} // namespace helios
