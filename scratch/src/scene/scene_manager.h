#pragma once

#include <memory>
#include <entity/entity.hpp>
#include <entity/id_factory.h>
#include <lights/directional_light.h>
#include <graphics/renderable.h>
#include "scene_node.h"
#include "camera/camera.h"
#include "scene.h"
#include <entt/entt.hpp>

namespace scratch {

    class SceneManager {
    public:
        SceneManager();

        unsigned int handleSelection(scratch::Shader &selectionShader, glm::vec2 mousePosition);

        void saveScene(std::string scenePath);

        void loadScene(std::string scenePath);

        scratch::Scene activeScene;

    private:
    };

}
