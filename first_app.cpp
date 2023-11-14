#include "first_app.hpp"

namespace helios {
void FirstApp::run() {
  while (!heliosWindow.shouldClose()) {
    glfwPollEvents();
  }
};
} // namespace helios
