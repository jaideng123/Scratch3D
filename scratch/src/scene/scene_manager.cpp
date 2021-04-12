//
// Created by JJJai on 3/13/2021.
//

#include <graphics/model_renderable.h>

#include <utility>
#include <graphics/render_system.h>
#include <main.h>
#include <include/rapidjson/prettywriter.h>
#include "scene_manager.h"
#include <fstream>
#include <include/rapidjson/document.h>

scratch::SceneManager::SceneManager() {
    _idFactory = scratch::IdFactory();
    _rootNode = scratch::SceneNode();
    _rootNode.setId(_idFactory.generateId());
}

std::shared_ptr<scratch::Renderable>
scratch::SceneManager::createModelRenderable(const std::string &modelPath,
                                             const std::shared_ptr<Shader> &defaultShader) {
    std::shared_ptr<scratch::Model> newModel = std::make_shared<scratch::Model>(_idFactory.generateId(), modelPath);
    for (auto material : newModel->getMaterials()) {
        material->setId(_idFactory.generateId());
        _materials.push_back(material);
    }
    newModel->setDefaultShader(defaultShader);
    _models.push_back(newModel);
    std::shared_ptr<scratch::Renderable> pRenderable = std::make_shared<scratch::ModelRenderable>(
            _idFactory.generateId(), newModel);
    _renderables.push_back(pRenderable);
    return pRenderable;

}


std::shared_ptr<scratch::Entity> scratch::SceneManager::createEntity(std::shared_ptr<Renderable> renderable) {
    std::shared_ptr<scratch::Entity> pEntity = std::make_shared<scratch::Entity>(_idFactory.generateId(), renderable);
    _entities.push_back(pEntity);
    return pEntity;
}

std::shared_ptr<scratch::SceneNode> scratch::SceneManager::createSceneNode(std::shared_ptr<Entity> entity) {
    std::shared_ptr<scratch::SceneNode> pNode = std::make_shared<scratch::SceneNode>();
    pNode->setEntity(entity);
    pNode->setId(_idFactory.generateId());
    return pNode;
}

std::shared_ptr<scratch::Shader>
scratch::SceneManager::createShader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::shared_ptr<scratch::Shader> shader = std::make_shared<scratch::Shader>(_idFactory.generateId(), vertexPath,
                                                                                fragmentPath);
    _shaders.push_back(shader);
    return shader;
}


void scratch::SceneManager::render(const scratch::Camera &camera) {
    std::vector<scratch::Mesh> renderQueue = std::vector<scratch::Mesh>();
    for (auto currentNode : _rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<scratch::Mesh> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            // TODO: get rid of these
            // also remove code from serialization in material
            mesh.getMaterial()->setMat4("model", modelMatrix);
            mesh.getMaterial()->setFloat("material.shininess", 32.0f);
            renderQueue.push_back(mesh);
        }
    }
    RenderSystem::render(renderQueue, *_directionalLight);
}

std::shared_ptr<scratch::DirectionalLight> scratch::SceneManager::createDirectionalLight() {
    _directionalLight = std::make_shared<scratch::DirectionalLight>();
    return _directionalLight;
}

std::shared_ptr<scratch::SceneNode> scratch::SceneManager::findSceneNode(unsigned int id) {
    for (auto currentNode : _rootNode.getChildren()) {
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
    glm::mat4 view = scratch::MainCamera->getViewMatrix();
    glm::mat4 projection = scratch::MainCamera->getProjectionMatrix();
    for (auto currentNode : _rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<scratch::Mesh> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            selectionShader.use();
            selectionShader.setMat4("model", modelMatrix);
            selectionShader.setMat4("view", view);
            selectionShader.setMat4("projection", projection);
            selectionShader.setUnsignedInt("entityId", currentNode->getId());
            mesh.draw();
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
    return _shaders;
}

scratch::SceneNode &scratch::SceneManager::getRootNode() {
    return _rootNode;
}

void scratch::SceneManager::saveScene(std::string scenePath) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("lastGeneratedId");
    writer.Uint(_idFactory.getLastGeneratedId());

    writer.String("shaders");
    writer.StartArray();
    for (auto &shader : _shaders) {
        shader->serialize(writer);
    }
    writer.EndArray();

    writer.String("materials");
    writer.StartArray();
    for (auto &material : _materials) {
        material->serialize(writer);
    }
    writer.EndArray();

    writer.String("models");
    writer.StartArray();
    for (auto &model : _models) {
        model->serialize(writer);
    }
    writer.EndArray();

    writer.String("renderables");
    writer.StartArray();
    for (auto &renderable : _renderables) {
        renderable->serialize(writer);
    }
    writer.EndArray();

    writer.String("entities");
    writer.StartArray();
    for (auto &entity : _entities) {
        entity->serialize(writer);
    }
    writer.EndArray();

    writer.String("directionalLight");
    _directionalLight->serialize(writer);

    writer.String("rootNode");
    _rootNode.serialize(writer);

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
    _idFactory.setLastGeneratedId(lastGeneratedId.GetUint());

    rapidjson::Value &materialsArray = document["materials"].GetArray();
    _materials.clear();
    for (rapidjson::Value::ConstValueIterator itr = materialsArray.Begin(); itr != materialsArray.End(); ++itr) {
        auto material = std::make_shared<scratch::Material>();
        material->deserialize(*itr);
        _materials.push_back(material);
    }

    rapidjson::Value &shadersArray = document["shaders"].GetArray();
    _shaders.clear();
    for (rapidjson::Value::ConstValueIterator itr = shadersArray.Begin(); itr != shadersArray.End(); ++itr) {
        std::shared_ptr<scratch::Shader> shader = std::make_shared<scratch::Shader>();
        shader->deserialize(*itr);
        _shaders.push_back(shader);
    }

    rapidjson::Value &modelsArray = document["models"].GetArray();
    _models.clear();
    for (rapidjson::Value::ConstValueIterator itr = modelsArray.Begin(); itr != modelsArray.End(); ++itr) {
        std::shared_ptr<scratch::Model> model = std::make_shared<scratch::Model>();
        model->deserialize(*itr);
        unsigned int defaultShaderId = (*itr)["defaultShaderId"].GetUint();
        std::shared_ptr<scratch::Shader> defaultShader;
        for (auto &shader : _shaders) {
            if (shader->getId() == defaultShaderId) {
                defaultShader = shader;
            }
        }
        model->setDefaultShader(defaultShader);
        // TODO: assert equal sizes between materials here
        auto materials = model->getMaterials();
        for (int i = 0; i < materials.size(); ++i) {
            unsigned int materialId = (*itr)["materialIds"].GetArray()[i].GetUint();
            std::shared_ptr<scratch::Material> targetMaterial;
            for (auto material : _materials) {
                if (material->getId() == materialId) {
                    targetMaterial = material;
                }
            }
            // Override materials
            materials[i]->setId(targetMaterial->getId());
            materials[i]->setShader(targetMaterial->getShader());
//            TODO: reinstate this once Material.deserialize is fixed
//            materials[i]->setParameters(targetMaterial->getParameters());
        }
        _models.push_back(model);
    }

    rapidjson::Value &renderablesArray = document["renderables"].GetArray();
    _renderables.clear();
    for (rapidjson::Value::ConstValueIterator itr = renderablesArray.Begin(); itr != renderablesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> newRenderable;
        if ((*itr)["type"].GetString() == scratch::ModelRenderable::TYPE) {
            // TODO: figure out how to move this into a deserialize function
            std::shared_ptr<scratch::Model> linkedModel;
            unsigned int targetModelId = (*itr)["modelId"].GetUint();
            for (auto &model : _models) {
                if (model->getId() == targetModelId) {
                    linkedModel = model;
                }
            }
            newRenderable = std::make_shared<scratch::ModelRenderable>((*itr)["id"].GetUint(), linkedModel);
        } else {
            throw std::runtime_error("Unexpected Renderable Type");
        }
        _renderables.push_back(newRenderable);
    }

    rapidjson::Value &entitiesArray = document["entities"].GetArray();
    _entities.clear();
    for (rapidjson::Value::ConstValueIterator itr = entitiesArray.Begin(); itr != entitiesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> linkedRenderable;
        unsigned int targetRenderableId = (*itr)["renderableId"].GetUint();
        for (auto &renderable : _renderables) {
            if (renderable->getId() == targetRenderableId) {
                linkedRenderable = renderable;
            }
        }
        std::shared_ptr<scratch::Entity> newEntity = std::make_shared<scratch::Entity>((*itr)["id"].GetUint(),
                                                                                       linkedRenderable);
        _entities.push_back(newEntity);
    }

    _directionalLight = std::make_shared<scratch::DirectionalLight>();
    _directionalLight->deserialize(document["directionalLight"]);

    _rootNode.deserialize(document["rootNode"], _entities);
    std::cout << "Finished Loading Scene" << std::endl;
}