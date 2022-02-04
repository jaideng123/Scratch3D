//
// Created by JJJai on 12/23/2021.
//

#include <main.h>
#include <graphics/model_renderable.h>
#include <graphics/render_system.h>
#include "scene.h"

scratch::Scene::Scene() {
    _idFactory = scratch::IdFactory();
    rootNode = scratch::SceneNode();
    rootNode.setId(_idFactory.generateId());
    _filePath = "";

    directionalLight = scratch::DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
                                                 scratch::Color(glm::vec3(0.2f)),
                                                 scratch::Color(glm::vec3(0.5f)),
                                                 scratch::WHITE);
}

std::shared_ptr<scratch::Entity> scratch::Scene::createEntity() {
    std::shared_ptr<scratch::Entity> pEntity = std::make_shared<scratch::Entity>(_idFactory.generateId(), this);
    _entities.push_back(pEntity);
    return pEntity;
}

std::shared_ptr<scratch::SceneNode> scratch::Scene::createSceneNode(std::shared_ptr<scratch::Entity> entity) {
    std::shared_ptr<scratch::SceneNode> pNode = std::make_shared<scratch::SceneNode>();
    pNode->setEntity(entity);
    pNode->setId(_idFactory.generateId());
    return pNode;
}


void scratch::Scene::render(const scratch::Camera &camera) const {
    for (auto currentNode : rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        if (currentEntity->hasComponent<scratch::ModelRenderable>()) {
            std::shared_ptr<ModelRenderable> renderableComponent = currentEntity->getComponent<scratch::ModelRenderable>();
            std::vector<std::shared_ptr<scratch::Mesh>> meshes = renderableComponent->getMeshes();
            std::vector<std::shared_ptr<scratch::Material>> materials = renderableComponent->getMaterials();
            glm::mat4 transformMatrix = currentNode->generateTransformMatrix();
            for (const auto & mesh : meshes) {
                scratch::RenderSystem::drawMesh(mesh, materials[mesh->getMaterialIndex()], transformMatrix);
            }
        }
    }
    RenderSystem::render(camera, directionalLight);
}

std::shared_ptr<scratch::SceneNode> scratch::Scene::findSceneNode(unsigned int id) const {
    for (auto currentNode : rootNode.getChildren()) {
        if (currentNode->getId() == id) {
            return currentNode;
        }
    }
    return nullptr;
}

const std::string &scratch::Scene::getFilePath() const {
    return _filePath;
}