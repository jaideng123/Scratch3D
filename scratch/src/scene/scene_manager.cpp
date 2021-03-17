//
// Created by JJJai on 3/13/2021.
//

#include <graphics/modelRenderable.h>

#include <utility>
#include <graphics/render_system.h>
#include <main.h>
#include <include/rapidjson/prettywriter.h>
#include "scene_manager.h"

scratch::SceneManager::SceneManager() {
    idFactory = scratch::IDFactory();
    rootNode = scratch::SceneNode();
    rootNode.setId(idFactory.generate_id());
}

std::shared_ptr<scratch::Renderable>
scratch::SceneManager::createModelRenderable(const std::string &modelPath, const std::shared_ptr<Shader> &shader) {
    std::shared_ptr<scratch::Model> newModel = std::make_shared<scratch::Model>(modelPath);
    setDefaultShader(newModel->getMeshes(), *shader);
    std::shared_ptr<scratch::Renderable> pRenderable = std::make_shared<scratch::ModelRenderable>(newModel);
    renderables.push_back(pRenderable);
    for (int i = 0; i < shaders.size(); ++i) {
        if (shaders[i] == shader) {
            return pRenderable;
        }
    }
    shaders.push_back(shader);
    return pRenderable;

}

void scratch::SceneManager::setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader &shader) {
    for (auto &mesh : meshes) {
        mesh.material->setShader(&shader);
    }
}

std::shared_ptr<scratch::Entity> scratch::SceneManager::createEntity(std::shared_ptr<Renderable> renderable) {
    std::shared_ptr<scratch::Entity> pEntity = std::make_shared<scratch::Entity>(renderable, idFactory.generate_id());
    entities.push_back(pEntity);
    return pEntity;
}

std::shared_ptr<scratch::SceneNode> scratch::SceneManager::createSceneNode(std::shared_ptr<Entity> entity) {
    std::shared_ptr<scratch::SceneNode> pNode = std::make_shared<scratch::SceneNode>();
    pNode->setEntity(entity);
    pNode->setId(idFactory.generate_id());
    return pNode;
}

void scratch::SceneManager::render(const scratch::Camera &camera) {
    std::vector<scratch::Mesh> renderQueue = std::vector<scratch::Mesh>();
    for (auto currentNode : rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<scratch::Mesh> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            mesh.material->setMat4("model", modelMatrix);
            mesh.material->setFloat("material.shininess", 32.0f);
            renderQueue.push_back(mesh);
        }
    }
    RenderSystem::render(renderQueue, *directionalLight);
}

std::shared_ptr<scratch::DirectionalLight> scratch::SceneManager::createDirectionalLight() {
    directionalLight = std::make_shared<scratch::DirectionalLight>();
    return directionalLight;
}

std::shared_ptr<scratch::SceneNode> scratch::SceneManager::findSceneNode(unsigned int id) {
    for (auto currentNode : rootNode.getChildren()) {
        if (currentNode->getId() == id) {
            return currentNode;
        }
    }
    return nullptr;
}

//TODO: Render to separate frame buffer
//TODO: break this off
unsigned int scratch::SceneManager::handleSelection(scratch::Shader &selectionShader, glm::vec2 mousePosition) {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = scratch::MainCamera->GetViewMatrix();
    glm::mat4 projection = scratch::MainCamera->GetProjectionMatrix();
    for (auto currentNode : rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<scratch::Mesh> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            selectionShader.use();
            selectionShader.setMat4("model", modelMatrix);
            selectionShader.setMat4("view", view);
            selectionShader.setMat4("projection", projection);
            selectionShader.setUnsignedInt("entityId", currentNode->getId());
            mesh.Draw();
        }
    }
    GLubyte pixel[3];
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glReadPixels(mousePosition.x, height - mousePosition.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
    unsigned int selectedId = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
    std::cout << "Selected Scene Node Id: " << selectedId << std::endl;

    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return selectedId;
}

const std::vector<std::shared_ptr<scratch::Shader>> &scratch::SceneManager::getShaders() const {
    return shaders;
}

scratch::SceneNode &scratch::SceneManager::getRootNode() {
    return rootNode;
}

void scratch::SceneManager::saveScene(std::string scenePath) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

    rootNode.serialize(writer);

    std::cout << sb.GetString() << std::endl;
}
