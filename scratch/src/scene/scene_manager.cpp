//
// Created by JJJai on 3/13/2021.
//

#include <graphics/model_renderable.h>

#include <graphics/render_system.h>
#include <main.h>
#include <fstream>

scratch::SceneManager::SceneManager() {
    _idFactory = scratch::IdFactory();
    _rootNode = scratch::SceneNode();
    _rootNode.setId(_idFactory.generateId());
    _currentSceneFilePath = "";
}

std::shared_ptr<scratch::Renderable>
scratch::SceneManager::createModelRenderable(const std::string &modelPath) {
    std::shared_ptr<scratch::Model> newModel = ScratchManagers->resourceManager->loadModel(modelPath);
    for (auto material : newModel->getDefaultMaterials()) {
        material->setId(_idFactory.generateId());
    }
    std::shared_ptr<scratch::Shader> defaultShader = ScratchManagers->shaderLibrary->findShader("Lit");
    newModel->setDefaultShader(defaultShader);
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


void scratch::SceneManager::render(const scratch::Camera &camera) {
    for (auto currentNode : _rootNode.getChildren()) {
        auto currentEntity = currentNode->getEntity();
        std::vector<std::shared_ptr<scratch::Mesh>> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 transformMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            scratch::RenderSystem::drawMesh(mesh,mesh->getMaterial(),transformMatrix);
        }
    }
    RenderSystem::render(camera, *_directionalLight);
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
        std::vector<std::shared_ptr<scratch::Mesh>> meshesToRender = currentEntity->getRenderable()->getMeshes();
        glm::mat4 modelMatrix = currentNode->generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            selectionShader.use();
            selectionShader.setMat4("model", modelMatrix);
            selectionShader.setMat4("view", view);
            selectionShader.setMat4("projection", projection);
            selectionShader.setUnsignedInt("entityId", currentNode->getId());
            mesh->draw();
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

scratch::SceneNode &scratch::SceneManager::getRootNode() {
    return _rootNode;
}

void scratch::SceneManager::saveScene(std::string scenePath) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

    writer.StartObject();

    std::cout << "Serializing Id Factory" << std::endl;
    writer.String("lastGeneratedId");
    writer.Uint(_idFactory.getLastGeneratedId());

    std::cout << "Serializing Renderables" << std::endl;
    writer.String("renderables");
    writer.StartArray();
    for (auto &renderable : _renderables) {
        renderable->serialize(writer);
    }
    writer.EndArray();

    std::cout << "Serializing Entities" << std::endl;
    writer.String("entities");
    writer.StartArray();
    for (auto &entity : _entities) {
        entity->serialize(writer);
    }
    writer.EndArray();

    std::cout << "Serializing Lights" << std::endl;
    writer.String("directionalLight");
    _directionalLight->serialize(writer);

    std::cout << "Serializing Scene Graph" << std::endl;
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

    std::cout << "Deserializing Id Factory State" << std::endl;
    rapidjson::Value &lastGeneratedId = document["lastGeneratedId"];
    _idFactory.setLastGeneratedId(lastGeneratedId.GetUint());

    std::cout << "Deserializing Renderables" << std::endl;
    rapidjson::Value &renderablesArray = document["renderables"].GetArray();
    _renderables.clear();
    for (rapidjson::Value::ConstValueIterator itr = renderablesArray.Begin(); itr != renderablesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> newRenderable;
        if ((*itr)["type"].GetString() == scratch::ModelRenderable::TYPE) {
            // TODO: figure out how to move this into a deserialize function
            std::string targetModelPath = (*itr)["modelPath"].GetString();
            std::shared_ptr<scratch::Model> linkedModel = ScratchManagers->resourceManager->loadModel(targetModelPath);
            std::vector<std::shared_ptr<scratch::Material>> materials = std::vector<std::shared_ptr<scratch::Material>>();
            rapidjson::GenericArray<true, rapidjson::GenericValue<rapidjson::UTF8<>>::ValueType> materialsArray = (*itr)["materials"].GetArray();
            for (rapidjson::Value::ConstValueIterator itrMaterials = materialsArray.Begin(); itrMaterials != materialsArray.End(); ++itrMaterials) {
                std::shared_ptr<scratch::Material> newMaterial = std::make_shared<scratch::Material>();
                newMaterial->deserialize(*itrMaterials);
                materials.push_back(newMaterial);
            }
            newRenderable = std::make_shared<scratch::ModelRenderable>((*itr)["id"].GetUint(), linkedModel, materials);
        } else {
            throw std::runtime_error("Unexpected Renderable Type");
        }
        _renderables.push_back(newRenderable);
    }

    std::cout << "Deserializing Entities" << std::endl;
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

    std::cout << "Deserializing Scene Graph" << std::endl;
    _rootNode.deserialize(document["rootNode"], _entities);

    std::cout << "Deserializing Lights" << std::endl;
    _directionalLight = std::make_shared<scratch::DirectionalLight>();
    _directionalLight->deserialize(document["directionalLight"]);

    _currentSceneFilePath = scenePath;

    std::cout << "Finished Loading Scene" << std::endl;
}

const std::string &scratch::SceneManager::getCurrentSceneFilePath() const {
    return _currentSceneFilePath;
}
