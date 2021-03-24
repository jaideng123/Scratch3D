//
// Created by JJJai on 3/13/2021.
//

#include <graphics/modelRenderable.h>

#include <utility>
#include <graphics/render_system.h>
#include <main.h>
#include <include/rapidjson/prettywriter.h>
#include "scene_manager.h"
#include <fstream>
#include <include/rapidjson/document.h>

scratch::SceneManager::SceneManager() {
    idFactory = scratch::IDFactory();
    rootNode = scratch::SceneNode();
    rootNode.setId(idFactory.generate_id());
}

std::shared_ptr<scratch::Renderable>
scratch::SceneManager::createModelRenderable(const std::string &modelPath,
                                             const std::shared_ptr<Shader> &defaultShader) {
    std::shared_ptr<scratch::Model> newModel = std::make_shared<scratch::Model>(idFactory.generate_id(), modelPath);
    newModel->setDefaultShader(defaultShader);
    models.push_back(newModel);
    std::shared_ptr<scratch::Renderable> pRenderable = std::make_shared<scratch::ModelRenderable>(
            idFactory.generate_id(), newModel);
    renderables.push_back(pRenderable);
    return pRenderable;

}


std::shared_ptr<scratch::Entity> scratch::SceneManager::createEntity(std::shared_ptr<Renderable> renderable) {
    std::shared_ptr<scratch::Entity> pEntity = std::make_shared<scratch::Entity>(idFactory.generate_id(), renderable);
    entities.push_back(pEntity);
    return pEntity;
}

std::shared_ptr<scratch::SceneNode> scratch::SceneManager::createSceneNode(std::shared_ptr<Entity> entity) {
    std::shared_ptr<scratch::SceneNode> pNode = std::make_shared<scratch::SceneNode>();
    pNode->setEntity(entity);
    pNode->setId(idFactory.generate_id());
    return pNode;
}

std::shared_ptr<scratch::Shader>
scratch::SceneManager::createShader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::shared_ptr<scratch::Shader> shader = std::make_shared<scratch::Shader>(idFactory.generate_id(), vertexPath,
                                                                                fragmentPath);
    shaders.push_back(shader);
    return shader;
}


void scratch::SceneManager::render(const scratch::Camera &camera) {
    std::vector<scratch::Mesh> renderQueue = std::vector<scratch::Mesh>();
    for (auto currentNode : rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<scratch::Mesh> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            // TODO: get rid of these
            // also remove code from serialization in material
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

    writer.StartObject();

    writer.String("lastGeneratedId");
    writer.Uint(idFactory.getLastGeneratedId());

    writer.String("shaders");
    writer.StartArray();
    for (auto &shader : shaders) {
        shader->serialize(writer);
    }
    writer.EndArray();

    writer.String("models");
    writer.StartArray();
    for (auto &model : models) {
        model->serialize(writer);
    }
    writer.EndArray();

    writer.String("renderables");
    writer.StartArray();
    for (auto &renderable : renderables) {
        renderable->serialize(writer);
    }
    writer.EndArray();

    writer.String("entities");
    writer.StartArray();
    for (auto &entity : entities) {
        entity->serialize(writer);
    }
    writer.EndArray();

    writer.String("directionalLight");
    directionalLight->serialize(writer);

    writer.String("rootNode");
    rootNode.serialize(writer);

    writer.EndObject();
    std::cout << "Saving Scene Description:" << std::endl;
    std::cout << sb.GetString() << std::endl;

    std::ofstream outfile;

    outfile.open(scenePath);
    std::cout << "Opened file: " << scenePath << std::endl;

    outfile << sb.GetString();
    outfile.close();
    std::cout << "Closed file: " << scenePath << std::endl;

}

void scratch::SceneManager::loadScene(std::string scenePath) {
    std::ifstream sceneFile;
    sceneFile.open(scenePath);
    std::string content((std::istreambuf_iterator<char>(sceneFile)),
                        (std::istreambuf_iterator<char>()));
    std::cout << "Read Scene file: " << content << std::endl;

    rapidjson::Document document;
    document.Parse(content.c_str());

    rapidjson::Value &lastGeneratedId = document["lastGeneratedId"];
    idFactory.setLastGeneratedId(lastGeneratedId.GetUint());

    rapidjson::Value &shadersArray = document["shaders"].GetArray();
    shaders.clear();
    for (rapidjson::Value::ConstValueIterator itr = shadersArray.Begin(); itr != shadersArray.End(); ++itr) {
        std::shared_ptr<scratch::Shader> shader = std::make_shared<scratch::Shader>();
        shader->deserialize(*itr);
        shaders.push_back(shader);
    }

    rapidjson::Value &modelsArray = document["models"].GetArray();
    models.clear();
    for (rapidjson::Value::ConstValueIterator itr = modelsArray.Begin(); itr != modelsArray.End(); ++itr) {
        std::shared_ptr<scratch::Model> model = std::make_shared<scratch::Model>();
        model->deserialize(*itr);
        unsigned int defaultShaderId = (*itr)["defaultShaderId"].GetUint();
        std::shared_ptr<scratch::Shader> defaultShader;
        for (auto &shader : shaders) {
            if (shader->Id == defaultShaderId) {
                defaultShader = shader;
            }
        }
        model->setDefaultShader(defaultShader);
        models.push_back(model);
    }

    rapidjson::Value &renderablesArray = document["renderables"].GetArray();
    renderables.clear();
    for (rapidjson::Value::ConstValueIterator itr = renderablesArray.Begin(); itr != renderablesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> newRenderable;
        if ((*itr)["type"].GetString() == scratch::ModelRenderable::TYPE) {
            // TODO: figure out how to move this into a deserialize function
            std::shared_ptr<scratch::Model> linkedModel;
            unsigned int targetModelId = (*itr)["modelId"].GetUint();
            for (auto &model : models) {
                if (model->Id == targetModelId) {
                    linkedModel = model;
                }
            }
            newRenderable = std::make_shared<scratch::ModelRenderable>((*itr)["id"].GetUint(), linkedModel);
        } else {
            throw std::runtime_error("Unexpected Renderable Type");
        }
        renderables.push_back(newRenderable);
    }

    rapidjson::Value &entitiesArray = document["entities"].GetArray();
    entities.clear();
    for (rapidjson::Value::ConstValueIterator itr = entitiesArray.Begin(); itr != entitiesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> linkedRenderable;
        unsigned int targetRenderableId = (*itr)["renderableId"].GetUint();
        for (auto &renderable : renderables) {
            if (renderable->getId() == targetRenderableId) {
                linkedRenderable = renderable;
            }
        }
        std::shared_ptr<scratch::Entity> newEntity = std::make_shared<scratch::Entity>((*itr)["id"].GetUint(),
                                                                                       linkedRenderable);
        entities.push_back(newEntity);
    }

    directionalLight = std::make_shared<scratch::DirectionalLight>();
    directionalLight->deserialize(document["directionalLight"]);

    rootNode.deserialize(document["rootNode"], entities);
    std::cout << "Finished Loading Scene" << std::endl;
}