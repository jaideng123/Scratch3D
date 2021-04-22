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
#include <utilities/assert.h>
#include "main.h"


void GLAPIENTRY messageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar *message,
                                const void *userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

void RenderSystem::setup() {
    // stbi_set_flip_vertically_on_load(true);
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    scratch::MainWindow = glfwCreateWindow(scratch::DEFAULT_WIDTH, scratch::DEFAULT_HEIGHT, "Scratch", nullptr, nullptr);
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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(scratch::MainWindow, true);
    const char *glslVersion = "#version 400 core";
    ImGui_ImplOpenGL3_Init(glslVersion);
}

void RenderSystem::render(const std::vector<scratch::Mesh> &renderQueue, scratch::DirectionalLight &directionalLight) {
    // Background Fill Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = scratch::MainCamera->getViewMatrix();
    glm::mat4 projection = scratch::MainCamera->getProjectionMatrix();
    glm::vec3 viewPosition = scratch::MainCamera->getPosition();

    std::optional<scratch::Material> currentMaterial = {};
    for (auto mesh : renderQueue) {
        if (!currentMaterial.has_value() || mesh.getMaterial()->getId() != currentMaterial.value().getId()) {
            currentMaterial = *mesh.getMaterial();
            currentMaterial.value().activate();
            currentMaterial.value().getShader()->setMat4("view", view);
            currentMaterial.value().getShader()->setMat4("projection", projection);
            currentMaterial.value().getShader()->setVec3("viewPos", viewPosition);
            directionalLight.applyToShader(*currentMaterial.value().getShader());
        }
        mesh.draw();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderSystem::startFrame() {
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glViewport(0, 0, width, height);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
//        bool showDemoWindow = true;
//        ImGui::ShowDemoWindow(&showDemoWindow);
    ImGuizmo::BeginFrame();
}

void RenderSystem::endFrame() {
    // Flip Buffers and Draw
    glfwSwapBuffers(scratch::MainWindow);
}
