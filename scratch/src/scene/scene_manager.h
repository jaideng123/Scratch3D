#pragma once

#include <memory>
#include <entity/entity.hpp>
#include <entity/id_factory.h>
#include <lights/directionalLight.h>
#include "scene_node.h"
#include "camera/camera.h"

namespace scratch {

    class SceneManager {
    public:
        SceneManager();

        std::shared_ptr<scratch::Renderable> createModelRenderable(const std::string &modelPath,
                                                                   const std::shared_ptr<Shader> &shader);

        std::shared_ptr<scratch::Entity> createEntity(std::shared_ptr<Renderable> renderable);

        std::shared_ptr<scratch::Shader> createShader(const std::string &vertexPath, const std::string &fragmentPath);

        std::shared_ptr<scratch::SceneNode> createSceneNode(std::shared_ptr<Entity> entity);

        std::shared_ptr<scratch::SceneNode> findSceneNode(unsigned int id);

        std::shared_ptr<scratch::DirectionalLight> createDirectionalLight();

        void render(const scratch::Camera &camera);

        unsigned int handleSelection(scratch::Shader &selectionShader, glm::vec2 mousePosition);

        const std::vector<std::shared_ptr<scratch::Shader>> &getShaders() const;

        SceneNode &getRootNode();

        void saveScene(std::string scenePath);

        void loadScene(std::string scenePath);

    private:
        scratch::IDFactory idFactory;
        scratch::SceneNode rootNode;
        std::vector<std::shared_ptr<scratch::Renderable>> renderables;
        std::vector<std::shared_ptr<scratch::Entity>> entities;
        std::vector<std::shared_ptr<scratch::Shader>> shaders;
        std::vector<std::shared_ptr<scratch::Model>> models;
        std::shared_ptr<scratch::DirectionalLight> directionalLight;
    };

}
