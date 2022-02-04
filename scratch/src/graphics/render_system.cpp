//
// Created by JJJai on 3/11/2021.
//

#include "render_system.h"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <optional>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include "main.h"

std::vector<scratch::RenderItem> scratch::RenderSystem::_renderQueue;

void GLAPIENTRY messageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam) {
    std::stringstream messageBuffer;
    messageBuffer << "---------------------opengl-callback-start------------" << std::endl;
    messageBuffer << "message: " << message << std::endl;
    messageBuffer << "type: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            messageBuffer << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            messageBuffer << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            messageBuffer << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            messageBuffer << "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            messageBuffer << "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            messageBuffer << "OTHER";
            break;
    }
    messageBuffer << std::endl;

    messageBuffer << "id: " << id << std::endl;
    messageBuffer << "severity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
            messageBuffer << "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            messageBuffer << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            messageBuffer << "HIGH";
            break;
        default:
            // Filter out messages without severity
            return;
    }
    messageBuffer << std::endl;
    messageBuffer << "---------------------opengl-callback-end--------------" << std::endl;
    std::cout << messageBuffer.str();
}

void scratch::RenderSystem::setup() {
    // stbi_set_flip_vertically_on_load(true);
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    scratch::MainWindow = glfwCreateWindow(scratch::DEFAULT_WIDTH, scratch::DEFAULT_HEIGHT, "Scratch", nullptr,
                                           nullptr);
    SCRATCH_ASSERT(scratch::MainWindow != nullptr);

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(scratch::MainWindow);

    gladLoadGL();
    fprintf(stdout, "OpenGL %s\n", glGetString(GL_VERSION));
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    // Enable Gamma correction (physically correct colors)
    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, nullptr);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup ImGui Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(scratch::MainWindow, true);
    const char *glslVersion = "#version 400 core";
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void scratch::RenderSystem::render(const scratch::Camera &camera, DirectionalLight directionalLight) {
    // Background Fill Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    glm::vec3 viewPosition = camera.getPosition();

    std::optional<scratch::Material> currentMaterial = {};
    for (const auto &renderItem : _renderQueue) {
        if(!currentMaterial.has_value() || currentMaterial.value().getId() != renderItem.materialRef->getId()) {
            currentMaterial = *renderItem.materialRef;
            currentMaterial.value().activate();
            currentMaterial.value().getShader()->setMat4("view", view);
            currentMaterial.value().getShader()->setMat4("projection", projection);
            currentMaterial.value().getShader()->setVec3("viewPos", viewPosition);
            directionalLight.applyToShader(*currentMaterial.value().getShader());
        }

        currentMaterial.value().getShader()->setMat4("model", renderItem.transform);

        renderItem.mesh->draw();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void scratch::RenderSystem::startFrame() {
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glViewport(0, 0, width, height);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    _renderQueue.clear();
}

void scratch::RenderSystem::endFrame() {
    // Flip Buffers and Draw
    glfwSwapBuffers(scratch::MainWindow);
}

void scratch::RenderSystem::drawMesh(const std::shared_ptr<scratch::Mesh> &mesh,
                                     const std::shared_ptr<scratch::Material> &materialRef,
                                     const glm::mat4 &transform) {
    // PERF: Sort by material
    _renderQueue.emplace_back(mesh, materialRef, transform);
}
