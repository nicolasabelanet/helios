#include "helios_window.hpp"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace helios {

HeliosWindow::HeliosWindow(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}

HeliosWindow::~HeliosWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void HeliosWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void HeliosWindow::createWindowSurface(VkInstance instance,
                                       VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface");
  }
}

void HeliosWindow::framebufferResizeCallback(GLFWwindow *window, int width,
                                             int height) {
  auto heliosWindow =
      reinterpret_cast<HeliosWindow *>(glfwGetWindowUserPointer(window));
  heliosWindow->frameBufferResized = true;
  heliosWindow->width = width;
  heliosWindow->height = height;
}

} // namespace helios
