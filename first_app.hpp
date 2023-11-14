#pragma once
#include "helios_device.hpp"
#include "helios_pipeline.hpp"
#include "helios_window.hpp"

namespace helios {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  void run();

private:
  HeliosWindow heliosWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  HeliosDevice heliosDevice{heliosWindow};
  HeliosPipeline heliosPipeline{
      heliosDevice, "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv",
      HeliosPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};
} // namespace helios
