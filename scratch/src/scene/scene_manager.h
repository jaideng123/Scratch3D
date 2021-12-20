#pragma once

#include <memory>
#include <entity/entity.hpp>
#include <entity/id_factory.h>
#include <lights/directional_light.h>
#include <graphics/renderable.h>
#include "scene_node.h"
#include "camera/camera.h"

namespace scratch {

    class SceneManager {
    public:
        SceneManager();

        std::shared_ptr<scratch::Renderable> createModelRenderable(const std::string &modelPath);

        std::shared_ptr<scratch::Entity> createEntity(std::shared_ptr<Renderable> renderable);


        std::shared_ptr<scratch::SceneNode> createSceneNode(std::shared_ptr<Entity> entity);

        std::shared_ptr<scratch::SceneNode> findSceneNode(unsigned int id);

        std::shared_ptr<scratch::DirectionalLight> createDirectionalLight();

        void render(const scratch::Camera &camera);

        unsigned int handleSelection(scratch::Shader &selectionShader, glm::vec2 mousePosition);

        SceneNode &getRootNode();

        void saveScene(std::string scenePath);

        void loadScene(std::string scenePath);

        const std::string &getCurrentSceneFilePath() const;

    private:
        std::string _currentSceneFilePath;
        scratch::IdFactory _idFactory;
        scratch::SceneNode _rootNode;

        std::vector<std::shared_ptr<scratch::Renderable>> _renderables;
        std::vector<std::shared_ptr<scratch::Entity>> _entities;
        std::shared_ptr<scratch::DirectionalLight> _directionalLight;
    };

}
