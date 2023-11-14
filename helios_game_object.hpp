#pragma once

#include "helios_model.hpp"
#include <memory>

namespace helios {

struct Transform2dComponent {
  glm::vec2 translation{};
  glm::vec2 scale{1.0f, 1.0f};
  float rotation;

  glm::mat2 mat2() {
    const float s = glm::sin(rotation);
    const float c = glm::cos(rotation);
    glm::mat2 rotMat{{c, s}, {-s, c}};
    glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
    return rotMat * scaleMat;
  };
};

class HeliosGameObject {

public:
  using id_t = unsigned int;

  static HeliosGameObject createGameObject() {
    static id_t currentId = 0;
    return HeliosGameObject(currentId++);
  }

  HeliosGameObject(const HeliosGameObject &) = delete;
  HeliosGameObject &operator=(const HeliosGameObject &) = delete;
  HeliosGameObject(HeliosGameObject &&) = default;
  HeliosGameObject &operator=(HeliosGameObject &&) = default;

  id_t getId() { return id; };

  std::shared_ptr<HeliosModel> model{};
  glm::vec3 color{};
  Transform2dComponent transform2d;

private:
  HeliosGameObject(id_t objId) : id{objId} {};

  id_t id;
};

} // namespace helios
