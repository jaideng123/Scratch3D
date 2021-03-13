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

// Local Headers
#include "entity/entity-factory.h"
#include "graphics/modelRenderable.h"
#include "time/scratch_time.h"
#include "graphics/render_system.h"


void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void handleInput();

void setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader &shader);

void handleSelection(scratch::Shader &selectionShader);

std::vector<scratch::Entity> entities = std::vector<scratch::Entity>();
std::vector<scratch::Shader *> shaders = std::vector<scratch::Shader *>();

double lastX = 400, lastY = 300;
bool firstMouse = true;

unsigned int selectedEntityId = 0;
bool checkSelection = false;

int main() {
    RenderSystem::setup();
    scratch::Time::InitializeClock();

    // Setup various input callbacks
    glfwSetCursorPosCallback(scratch::MainWindow, mouse_callback);
    glfwSetKeyCallback(scratch::MainWindow, keyCallback);
    glfwSetMouseButtonCallback(scratch::MainWindow, mouse_button_callback);


    std::cout << "Loading Shaders..." << std::endl;
    scratch::Shader unlitShader = scratch::Shader("./shaders/unlit.vert", "./shaders/unlit.frag");
    shaders.push_back(&unlitShader);
    scratch::Shader litShader = scratch::Shader("./shaders/lit.vert", "./shaders/lit.frag");
    shaders.push_back(&litShader);
    scratch::Shader selectionShader = scratch::Shader("./shaders/entity-selection.vert",
                                                      "./shaders/entity-selection.frag");
    shaders.push_back(&selectionShader);

    std::cout << "Loading Models..." << std::endl;
    scratch::Model nanosuitModel = scratch::Model("./models/nanosuit/nanosuit.obj");
    setDefaultShader(nanosuitModel.getMeshes(), litShader);
    scratch::Model stoneModel = scratch::Model("./models/stone-man/Stone.obj");
    setDefaultShader(stoneModel.getMeshes(), litShader);

    std::cout << "Creating Entities..." << std::endl;
    scratch::EntityFactory entityFactory = scratch::EntityFactory();
    entities.push_back(
            entityFactory.create_entity(glm::vec3(-2, 0, 0), glm::vec3(0.2f),
                                        new scratch::ModelRenderable(nanosuitModel)));
    entities.push_back(
            entityFactory.create_entity(glm::vec3(0, 0, 0), glm::vec3(0.2f),
                                        new scratch::ModelRenderable(stoneModel)));

    selectedEntityId = 0;

    scratch::DirectionalLight directionalLight = scratch::DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
                                                                           scratch::Color(glm::vec3(0.2f)),
                                                                           scratch::Color(glm::vec3(0.5f)),
                                                                           scratch::WHITE);

    scratch::MainCamera = new scratch::Camera();

    scratch::TransformGizmo transformGizmo = scratch::TransformGizmo(scratch::MainCamera);

    std::cout << "starting rendering loop" << std::endl;
    // Rendering Loop
    while (glfwWindowShouldClose(scratch::MainWindow) == false) {
        glfwPollEvents();
        scratch::Time::UpdateClock();

        handleInput();

        RenderSystem::startFrame();

        // Edit Transform
        if (selectedEntityId != 0) {
            glm::mat4 matrix = entities[selectedEntityId - 1].generateTransformMatrix();
            transformGizmo.setCurrentTransform(matrix);
            transformGizmo.render();
            entities[selectedEntityId - 1].setTransform(transformGizmo.getCurrentTransform());
        }

        if (checkSelection) {
            handleSelection(selectionShader);
            checkSelection = false;
        }

        std::vector<scratch::Mesh> renderQueue = std::vector<scratch::Mesh>();
        for (size_t i = 0; i < entities.size(); i++) {
            std::vector<scratch::Mesh> meshesToRender = entities[i].getRenderable()->getMeshes();
            bool highlighted = (selectedEntityId == i + 1);
            glm::mat4 modelMatrix = entities[i].generateTransformMatrix();
            for (auto &mesh : meshesToRender) {
                mesh.material->setBool("highlighted", highlighted);
                mesh.material->setMat4("model", modelMatrix);
                mesh.material->setFloat("material.shininess", 32.0f);
                renderQueue.push_back(mesh);
            }
        }

        RenderSystem::render(renderQueue, directionalLight);

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
        for (auto &shader : shaders) {
            shader->reload();
        }
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

void setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader &shader) {
    for (auto &mesh : meshes) {
        mesh.material->setShader(&shader);
    }
}

//TODO: Render to separate frame buffer
//TODO: break this off
void handleSelection(scratch::Shader &selectionShader) {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 view = scratch::MainCamera->GetViewMatrix();
    glm::mat4 projection = scratch::MainCamera->GetProjectionMatrix();
    for (size_t i = 0; i < entities.size(); i++) {
        std::vector<scratch::Mesh> meshesToRender = entities[i].getRenderable()->getMeshes();
        glm::mat4 modelMatrix = entities[i].generateTransformMatrix();
        for (auto &mesh : meshesToRender) {
            selectionShader.use();
            selectionShader.setMat4("model", modelMatrix);
            selectionShader.setMat4("view", view);
            selectionShader.setMat4("projection", projection);
            selectionShader.setUnsignedInt("entityId", entities[i].getID());
            mesh.Draw();
        }
    }
    GLubyte pixel[3];
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glReadPixels(lastX, height - lastY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
    selectedEntityId = pixel[0] | pixel[1] << 8 | pixel[2] << 16;
    std::cout << "Selected Entity Id: " << selectedEntityId << std::endl;

    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}