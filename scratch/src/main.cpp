// Local Headers
#include "main.h"

// Dear IMGUI Headers
#include "imgui.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <optional>
#include <gui/transform_gizmo.h>
#include <gui/main_menu_bar.h>
#include <scene/scene_manager.h>

// Local Headers
#include "time/scratch_time.h"
#include "graphics/render_system.h"


void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void handleInput();

double lastX = 400, lastY = 300;
bool firstMouse = true;

unsigned int selectedSceneNodeId = 0;
bool checkSelection = false;

scratch::SceneManager sceneManager = scratch::SceneManager();

int main() {
    RenderSystem::setup();
    scratch::Time::InitializeClock();

    // Setup various input callbacks
    glfwSetCursorPosCallback(scratch::MainWindow, mouse_callback);
    glfwSetKeyCallback(scratch::MainWindow, keyCallback);
    glfwSetMouseButtonCallback(scratch::MainWindow, mouse_button_callback);


    std::cout << "Loading Shaders..." << std::endl;
    auto unlitShader = std::make_shared<scratch::Shader>("./assets/shaders/unlit.vert", "./assets/shaders/unlit.frag");
    auto litShader = std::make_shared<scratch::Shader>("./assets/shaders/lit.vert", "./assets/shaders/lit.frag");
    auto selectionShader = std::make_shared<scratch::Shader>("./assets/shaders/entity-selection.vert",
                                                             "./assets/shaders/entity-selection.frag");

    std::cout << "Loading Models..." << std::endl;
    auto nanoSuitModel = sceneManager.createModelRenderable("./assets/models/nanosuit/nanosuit.obj", litShader);
    auto stoneManModel = sceneManager.createModelRenderable("./assets/models/stone-man/Stone.obj", litShader);

    std::cout << "Creating Entities..." << std::endl;
    auto nanosuitEntity = sceneManager.createEntity(nanoSuitModel);
    auto stoneManEntity = sceneManager.createEntity(stoneManModel);

    std::cout << "Creating Scene Nodes..." << std::endl;
    auto nanosuitNode = sceneManager.createSceneNode(nanosuitEntity);
    nanosuitNode->setPosition(glm::vec3(-2, 0, 0));
    nanosuitNode->setScale(glm::vec3(0.2f));
    sceneManager.getRootNode().addChild(nanosuitNode);

    auto stoneManNode = sceneManager.createSceneNode(stoneManEntity);
    stoneManNode->setScale(glm::vec3(0.2f));
    sceneManager.getRootNode().addChild(stoneManNode);


    selectedSceneNodeId = 0;

    auto directionalLight = sceneManager.createDirectionalLight();
    directionalLight->setDirection(glm::vec3(-0.2f, -1.0f, -0.3f));
    directionalLight->setAmbient(scratch::Color(glm::vec3(0.2f)));
    directionalLight->setDiffuse(scratch::Color(glm::vec3(0.5f)));
    directionalLight->setSpecular(scratch::WHITE);


    scratch::MainCamera = new scratch::Camera();

    scratch::TransformGizmo transformGizmo = scratch::TransformGizmo(scratch::MainCamera);

    scratch::MainMenuBar mainMenuBar = scratch::MainMenuBar();

    std::cout << "starting rendering loop" << std::endl;
    // Rendering Loop
    while (glfwWindowShouldClose(scratch::MainWindow) == false) {
        glfwPollEvents();
        scratch::Time::UpdateClock();

        handleInput();

        RenderSystem::startFrame();
        auto selectedNode = selectedSceneNodeId == 0 ? nullptr : sceneManager.findSceneNode(selectedSceneNodeId);
        if (selectedNode != nullptr) {
            glm::mat4 matrix = selectedNode->generateTransformMatrix();
            transformGizmo.setCurrentTransform(matrix);
            transformGizmo.render();
            selectedNode->setTransform(transformGizmo.getCurrentTransform());
        }

        if (checkSelection) {
            selectedSceneNodeId = sceneManager.handleSelection(*selectionShader, glm::vec2(lastX, lastY));
            checkSelection = false;
        }

        mainMenuBar.render();

        sceneManager.render(*scratch::MainCamera);

        RenderSystem::endFrame();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}


// glfw: whenever the mouse is clicked, this callback is called
// -------------------------------------------------------
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        checkSelection = true;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        return;
    }

    scratch::MainCamera->ProcessMouseMovement(xoffset, yoffset);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        for (auto &shader : sceneManager.getShaders()) {
            shader->reload();
        }
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        sceneManager.saveScene("");
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
            scratch::MainCamera->ProcessKeyboard(scratch::FORWARD, scratch::Time::GetDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_S) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::BACKWARD, scratch::Time::GetDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_A) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::LEFT, scratch::Time::GetDeltaTime());
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_D) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::RIGHT, scratch::Time::GetDeltaTime());
    } else {
        glfwSetInputMode(scratch::MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}