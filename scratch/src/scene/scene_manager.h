#pragma once

#include <memory>
#include <entity/entity.hpp>
#include "scene_node.h"
#include "camera/camera.h"

namespace scratch {

    class SceneManager {
    public:
        SceneManager();

        std::shared_ptr<scratch::Renderable> createModelRenderable(std::string modelPath);
        std::shared_ptr<scratch::Entity> createEntity(std::shared_ptr<Renderable> renderable);
        std::shared_ptr<scratch::SceneNode> createSceneNode(std::shared_ptr<Entity> entity);
    private:
        scratch::SceneNode rootNode;
    };

}
