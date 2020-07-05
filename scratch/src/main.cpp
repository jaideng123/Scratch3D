// Local Headers
#include "main.hpp"
#include "shader/shader.h"
#include "model/model.hpp"
#include "camera/camera.hpp"
#include "entity/entity.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <optional>

Camera camera = Camera();
std::vector<scratch::Entity> entities = std::vector<scratch::Entity>();

std::vector<scratch::Shader *> shaders = std::vector<scratch::Shader *>();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = 400, lastY = 300;
bool firstMouse = true;
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    {
        return;
    }

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int selectedEntityIndex = 0;
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        selectedEntityIndex = (selectedEntityIndex + 1) % entities.size();
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        for (size_t i = 0; i < shaders.size(); i++)
        {
            shaders[i]->reload();
        }
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    float moveSpeed = 4.0f;
    scratch::Entity *selectedEntity = &entities[selectedEntityIndex];
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(0, 0, 1) * deltaTime * moveSpeed));
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(0, 0, -1) * deltaTime * moveSpeed));
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(0, 1, 0) * deltaTime * moveSpeed));
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(0, -1, 0) * deltaTime * moveSpeed));
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(-1, 0, 0) * deltaTime * moveSpeed));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(1, 0, 0) * deltaTime * moveSpeed));
}

void setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader shader)
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].material->setShader(&shader);
    }
}

int main()
{
    // stbi_set_flip_vertically_on_load(true);
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "Scratch", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr)
    {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stdout, "OpenGL %s\n", glGetString(GL_VERSION));

    // hide + capture cursor
    // get cursor input
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetKeyCallback(mWindow, keyCallback);
    glViewport(0, 0, mWidth, mHeight);

    std::cout << "Loading Shaders..." << std::endl;
    scratch::Shader unlitShader = scratch::Shader("./scratch/shaders/unlit.vert", "./scratch/shaders/unlit.frag");
    shaders.push_back(&unlitShader);

    std::cout << "Loading Models..." << std::endl;
    scratch::Model nanosuitModel = scratch::Model("./scratch/models/nanosuit/nanosuit.obj");
    setDefaultShader(nanosuitModel.getMeshes(), unlitShader);
    scratch::Model stoneModel = scratch::Model("./scratch/models/stone-man/Stone.obj");
    setDefaultShader(stoneModel.getMeshes(), unlitShader);

    // entities.push_back(scratch::Entity(glm::vec3(0, 0, 0), glm::vec3(0.2f), nanosuitModel));
    entities.push_back(scratch::Entity(glm::vec3(0, 0, 0), glm::vec3(0.2f), stoneModel));
    entities.push_back(scratch::Entity(glm::vec3(0, 0, 0), glm::vec3(0.2f), nanosuitModel));

    selectedEntityIndex = 0;

    glEnable(GL_DEPTH_TEST);

    std::cout << "starting rendering loop" << std::endl;
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_2))
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective<double>(glm::radians(60.0f), mWidth / mHeight, 0.1f, 100.0f);

        std::vector<scratch::Mesh> renderQueue = std::vector<scratch::Mesh>();
        for (size_t i = 0; i < entities.size(); i++)
        {
            std::vector<scratch::Mesh> toAdd = entities[i].getModel().getMeshes();
            bool highlighted = (i == selectedEntityIndex);
            glm::mat4 modelMatrix = entities[i].generateTransformMatrix();
            for (size_t j = 0; j < toAdd.size(); j++)
            {
                toAdd[j].material->setBool("highlighted", highlighted);
                toAdd[j].material->setMat4("model", modelMatrix);
                toAdd[j].material->setMat4("view", view);
                toAdd[j].material->setMat4("projection", projection);
                renderQueue.push_back(toAdd[j]);
            }
        }

        std::optional<scratch::Material> currentMaterial = {};
        for (size_t i = 0; i < renderQueue.size(); i++)
        {
            if (!currentMaterial.has_value() || renderQueue[i].material->ID != currentMaterial.value().ID)
            {
                currentMaterial = *renderQueue[i].material;
                currentMaterial.value().activate();
                // TODO: fix issues with view matrix serialization
                renderQueue[i].material->getShader()->setMat4("view", view);
            }
            renderQueue[i].Draw();
        }

        processInput(mWindow);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
