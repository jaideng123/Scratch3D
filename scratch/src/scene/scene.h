#pragma once

#include <entity/id_factory.h>
#include <vector>
#include <lights/directional_light.h>
#include <camera/camera.h>
#include "scene_node.h"
#include "scene_manager.h"

namespace scratch {
    class Scene {
    public:

        Scene();

        std::shared_ptr<scratch::Renderable> createModelRenderable(const std::string &modelPath);
        std::shared_ptr<scratch::Entity> createEntity(std::shared_ptr<Renderable> renderable);
        std::shared_ptr<scratch::SceneNode> createSceneNode(std::shared_ptr<Entity> entity);
        std::shared_ptr<scratch::SceneNode> findSceneNode(unsigned int id) const;
        std::shared_ptr<scratch::DirectionalLight> createDirectionalLight();
        void render(const scratch::Camera &camera) const;
        const std::string &getFilePath() const;

        scratch::SceneNode rootNode;
        scratch::DirectionalLight directionalLight;

    private:
        std::string _filePath;

        std::vector<std::shared_ptr<scratch::Renderable>> _renderables;
        std::vector<std::shared_ptr<scratch::Entity>> _entities;

        scratch::IdFactory _idFactory;

        friend class SceneManager;
    };
};