#pragma once

#include "helios_model.hpp"

// std
#include <memory>

// lib
#include "glm/gtc/matrix_transform.hpp"

namespace helios {

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  glm::vec3 rotation{};

  glm::mat4 mat4();
  glm::mat3 normalMatrix();
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
  TransformComponent transform;

private:
  HeliosGameObject(id_t objId) : id{objId} {};

  id_t id;
};

} // namespace helios
