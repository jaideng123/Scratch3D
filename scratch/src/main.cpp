// Local Headers
#include "main.hpp"
#include "shader/shader.h"
#include "model/model.hpp"
#include "camera/camera.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

Camera camera = Camera();

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 2.5f * deltaTime;
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

int main()
{
    stbi_set_flip_vertically_on_load(true);
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
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // get cursor input
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glViewport(0, 0, mWidth, mHeight);

    std::cout << "Loading Shaders..." << std::endl;
    Scratch::Shader unlitShader = Scratch::Shader("./scratch/shaders/unlit.vert", "./scratch/shaders/unlit.frag");

    std::cout << "Loading Models..." << std::endl;
    Scratch::Model nanosuitModel = Scratch::Model("./scratch/models/nanosuit/nanosuit.obj");

    std::cout
        << "starting rendering loop";
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective<double>(glm::radians(45.0f), mWidth / mHeight, 0.1f, 100.0f);
        unlitShader.use();
        unlitShader.setMat4("view", view);
        unlitShader.setMat4("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(.2f));
        model = glm::translate(model, glm::vec3(0, 0, 0));
        unlitShader.setMat4("model", model);
        nanosuitModel.Draw(unlitShader);
        processInput(mWindow);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
