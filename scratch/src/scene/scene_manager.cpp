//
// Created by JJJai on 3/13/2021.
//

#include <graphics/model_renderable.h>

#include <graphics/render_system.h>
#include <main.h>
#include <fstream>

scratch::SceneManager::SceneManager() {
    activeScene = Scene();
}

//TODO: break this off & Render to separate frame buffer
unsigned int scratch::SceneManager::handleSelection(scratch::Shader &selectionShader, glm::vec2 mousePosition) {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = scratch::MainCamera->getViewMatrix();
    glm::mat4 projection = scratch::MainCamera->getProjectionMatrix();
    for (auto currentNode : activeScene.rootNode.getChildren()) {
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

void scratch::SceneManager::saveScene(std::string scenePath) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

    writer.StartObject();

    std::cout << "Serializing Id Factory" << std::endl;
    writer.String("lastGeneratedId");
    writer.Uint(activeScene._idFactory.getLastGeneratedId());

    std::cout << "Serializing Renderables" << std::endl;
    writer.String("renderables");
    writer.StartArray();
    for (auto &renderable : activeScene._renderables) {
        renderable->serialize(writer);
    }
    writer.EndArray();

    std::cout << "Serializing Entities" << std::endl;
    writer.String("entities");
    writer.StartArray();
    for (auto &entity : activeScene._entities) {
        entity->serialize(writer);
    }
    writer.EndArray();

    std::cout << "Serializing Lights" << std::endl;
    writer.String("directionalLight");
    activeScene.directionalLight.serialize(writer);

    std::cout << "Serializing Scene Graph" << std::endl;
    writer.String("rootNode");
    activeScene.rootNode.serialize(writer);

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
    activeScene = Scene();
    std::ifstream sceneFile;
    sceneFile.open(scenePath);
    std::string content((std::istreambuf_iterator<char>(sceneFile)),
                        (std::istreambuf_iterator<char>()));
    std::cout << "Read Scene file: " << content << std::endl;

    rapidjson::Document document;
    document.Parse(content.c_str());

    std::cout << "Deserializing Id Factory State" << std::endl;
    rapidjson::Value &lastGeneratedId = document["lastGeneratedId"];
    activeScene._idFactory.setLastGeneratedId(lastGeneratedId.GetUint());

    std::cout << "Deserializing Renderables" << std::endl;
    rapidjson::Value &renderablesArray = document["renderables"].GetArray();
    for (rapidjson::Value::ConstValueIterator itr = renderablesArray.Begin(); itr != renderablesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> newRenderable;
        if ((*itr)["type"].GetString() == scratch::ModelRenderable::TYPE) {
            // TODO: Move this into a deserialize function
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
        activeScene._renderables.push_back(newRenderable);
    }

    std::cout << "Deserializing Entities" << std::endl;
    rapidjson::Value &entitiesArray = document["entities"].GetArray();
    for (rapidjson::Value::ConstValueIterator itr = entitiesArray.Begin(); itr != entitiesArray.End(); ++itr) {
        std::shared_ptr<scratch::Renderable> linkedRenderable;
        unsigned int targetRenderableId = (*itr)["renderableId"].GetUint();
        for (auto &renderable : activeScene._renderables) {
            if (renderable->getId() == targetRenderableId) {
                linkedRenderable = renderable;
            }
        }
        std::shared_ptr<scratch::Entity> newEntity = std::make_shared<scratch::Entity>((*itr)["id"].GetUint(),
                                                                                       linkedRenderable);
        activeScene._entities.push_back(newEntity);
    }

    std::cout << "Deserializing Scene Graph" << std::endl;
    activeScene.rootNode.deserialize(document["rootNode"], activeScene._entities);

    std::cout << "Deserializing Lights" << std::endl;
    activeScene.directionalLight = scratch::DirectionalLight();
    activeScene.directionalLight.deserialize(document["directionalLight"]);

    activeScene._filePath = scenePath;

    std::cout << "Finished Loading Scene" << std::endl;
}