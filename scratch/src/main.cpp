// Local Headers
#include "main.h"

// Dear IMGUI Headers
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>


// Standard Headers
#include <cstdlib>
#include <iostream>
#include <optional>
#include <gui/transform_gizmo.h>
#include <gui/scene_heirarchy.h>
#include <gui/main_menu_bar.h>
#include <backends/imgui_impl_glfw.h>
#include <gui/material_props_widget.h>
#include <filesystem>
#include <graphics/model_renderable.h>
#include "xxhash.h"

// Local Headers
#include "time/scratch_time.h"
#include "graphics/render_system.h"


void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void mouseCallback(GLFWwindow *window, double xpos, double ypos);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void handleInput();

void loadDefaultScene();

double lastX = 400, lastY = 300;
bool firstMouse = true;

unsigned int selectedSceneNodeId = 0;
bool checkSelection = false;


int main() {
    scratch::RenderSystem::setup();
    scratch::Time::initializeClock();
    scratch::ScratchManagers = new scratch::Managers();

    // Setup various input callbacks
    glfwSetCursorPosCallback(scratch::MainWindow, mouseCallback);
    glfwSetKeyCallback(scratch::MainWindow, keyCallback);
    glfwSetMouseButtonCallback(scratch::MainWindow, mouseButtonCallback);

    std::cout << "Loading Shaders..." << std::endl;
    auto selectionShader = scratch::ScratchManagers->shaderLibrary->addShader(
            "EntitySelection",
            "./assets/shaders/entity-selection.vert",
            "./assets/shaders/entity-selection.frag");
    auto unlitShader = scratch::ScratchManagers->shaderLibrary->addShader("Unlit",
                                                                          "./assets/shaders/unlit.vert",
                                                                          "./assets/shaders/unlit.frag");
    auto litShader = scratch::ScratchManagers->shaderLibrary->addShader("Lit",
                                                                        "./assets/shaders/lit.vert",
                                                                        "./assets/shaders/lit.frag");

    loadDefaultScene();
    selectedSceneNodeId = 0;

    scratch::MainCamera = new scratch::Camera();

    auto transformGizmo = scratch::TransformGizmo(scratch::MainCamera);

    auto sceneHeirarchyGizmo = scratch::SceneHeirarchy(scratch::ScratchManagers->sceneManager->activeScene.rootNode);

    auto materialPropsWidget = scratch::MaterialPropsWidget();

    scratch::MainMenuBar mainMenuBar = scratch::MainMenuBar();

    std::cout << "starting rendering loop" << std::endl;

    // Rendering Loop
    while (glfwWindowShouldClose(scratch::MainWindow) == false) {
        glfwPollEvents();
        scratch::Time::updateClock();

        handleInput();

        scratch::RenderSystem::startFrame();

        auto selectedNode =
                selectedSceneNodeId == 0 ? nullptr : scratch::ScratchManagers->sceneManager->activeScene.findSceneNode(
                        selectedSceneNodeId);
        if (selectedNode != nullptr) {
            ImGui::SetNextWindowPos(ImVec2(0, 250.0f), ImGuiCond_Once);
            ImGui::Begin("Selected Scene Node");
            std::string currentName = selectedNode->getName();
            ImGui::InputText("##NODE-NAME", &currentName);
            selectedNode->setName(currentName);
            if (ImGui::CollapsingHeader("Transform Edit", ImGuiTreeNodeFlags_DefaultOpen)) {
                glm::mat4 matrix = selectedNode->generateTransformMatrix();
                transformGizmo.setCurrentTransform(matrix);
                transformGizmo.render();
                selectedNode->setTransform(transformGizmo.getCurrentTransform());
            }
            if (selectedNode->getEntity()->hasComponent<scratch::ModelRenderable>()) {
                if (ImGui::CollapsingHeader("Material Props", ImGuiTreeNodeFlags_DefaultOpen)) {
                    materialPropsWidget.setMaterials(
                            selectedNode->getEntity()->getComponent<scratch::ModelRenderable>()->getMaterials());
                    materialPropsWidget.render();
                }
            }

            ImGui::End();
        }

        sceneHeirarchyGizmo.setRootNode(scratch::ScratchManagers->sceneManager->activeScene.rootNode);
        sceneHeirarchyGizmo.setSelectedNode(selectedNode);
        if (scratch::ScratchManagers->sceneManager->activeScene.getFilePath().empty()) {
            sceneHeirarchyGizmo.setSceneName("New Scene");
        } else {
            std::string currentScenePath = scratch::ScratchManagers->sceneManager->activeScene.getFilePath();
            std::filesystem::path actualPath = std::filesystem::path(currentScenePath);
            sceneHeirarchyGizmo.setSceneName(actualPath.filename().string());
        }
        sceneHeirarchyGizmo.render();
        if (sceneHeirarchyGizmo.getSelectedNode() != nullptr) {
            selectedSceneNodeId = sceneHeirarchyGizmo.getSelectedNode()->getId();
        }

        if (checkSelection) {
            selectedSceneNodeId = scratch::ScratchManagers->sceneManager->handleSelection(*selectionShader,
                                                                                          glm::vec2(lastX, lastY));
            checkSelection = false;
        }

        mainMenuBar.render();

        scratch::ScratchManagers->sceneManager->activeScene.render(*scratch::MainCamera);

        scratch::RenderSystem::endFrame();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}


// glfw: whenever the mouse is clicked, this callback is called
// -------------------------------------------------------
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        checkSelection = true;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        return;
    }

    scratch::MainCamera->processMouseMovement(xoffset, yoffset);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        return;
    }
}

void handleInput() {
    if (glfwGetKey(scratch::MainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(scratch::MainWindow, true);
    }
    if (ImGui::GetIO().WantCaptureMouse) {
        glfwSetInputMode(scratch::MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    if (glfwGetMouseButton(scratch::MainWindow, GLFW_MOUSE_BUTTON_2)) {
        glfwSetInputMode(scratch::MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(scratch::MainWindow, true);
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_W) == GLFW_PRESS)
            scratch::MainCamera->processKeyboard(scratch::FORWARD, scratch::Time::getDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_S) == GLFW_PRESS)
            scratch::MainCamera->processKeyboard(scratch::BACKWARD, scratch::Time::getDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_A) == GLFW_PRESS)
            scratch::MainCamera->processKeyboard(scratch::LEFT, scratch::Time::getDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_D) == GLFW_PRESS)
            scratch::MainCamera->processKeyboard(scratch::RIGHT, scratch::Time::getDeltaTime());
    } else {
        glfwSetInputMode(scratch::MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void loadDefaultScene() {

    std::cout << "Creating Stone Men..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        auto stoneManModel = scratch::ScratchManagers->resourceManager->loadModel("./assets/models/stone-man/Stone.obj");
        auto stoneManEntity = scratch::ScratchManagers->sceneManager->activeScene.createEntity();
        auto renderableComponent = stoneManEntity->addComponent<scratch::ModelRenderable>(stoneManModel);
        for (const auto &material : renderableComponent->getMaterials()) {
            material->setFloat("material.shininess", 32);
        }
        auto stoneManNode = scratch::ScratchManagers->sceneManager->activeScene.createSceneNode(stoneManEntity);
        stoneManNode->setName("Stone Man - " + std::to_string(i));
        stoneManNode->setPosition(glm::vec3(i * .5, 0, 0));
        stoneManNode->setScale(glm::vec3(0.2f));
        scratch::ScratchManagers->sceneManager->activeScene.rootNode.addChild(stoneManNode);
        std::cout << "Created: " << stoneManNode->getName() << std::endl;
    }

    std::cout << "Creating Suit men..." << std::endl;
//    for (int i = 0; i < 2; ++i) {
//        auto nanoSuitModel = scratch::ScratchManagers->sceneManager->activeScene.createModelRenderable(
//                "./assets/models/nanosuit/nanosuit.obj");
//        auto nanosuitEntity = scratch::ScratchManagers->sceneManager->activeScene.createEntity(nanoSuitModel);
//        for (const auto &material : nanosuitEntity->getRenderable()->getMaterials()) {
//            material->setFloat("material.shininess", 32);
//        }
//        auto nanosuitNode = scratch::ScratchManagers->sceneManager->activeScene.createSceneNode(nanosuitEntity);
//        nanosuitNode->setName("Nano suit - " + std::to_string(i));
//        nanosuitNode->setPosition(glm::vec3(-2 + i * .1, 0, 0));
//        nanosuitNode->setScale(glm::vec3(0.2f));
//        scratch::ScratchManagers->sceneManager->activeScene.rootNode.addChild(nanosuitNode);
//    }
}