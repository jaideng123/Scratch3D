//
// Created by JJJai on 3/11/2021.
//

#include "render_system.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cstdio>
#include <optional>
#include <lights/directionalLight.h>
#include "main.h"
#include "mesh.hpp"


void GLAPIENTRY MessageCallback(GLenum source,
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
    gladLoadGL();
    fprintf(stdout, "OpenGL %s\n", glGetString(GL_VERSION));
    int width, height;
    glfwGetWindowSize(scratch::MainWindow, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    // Enable Gamma correction (physically correct colors)
    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);
}

void RenderSystem::render(const std::vector<scratch::Mesh> &renderQueue, scratch::DirectionalLight &directionalLight) {
    // Background Fill Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = scratch::MainCamera->GetViewMatrix();
    glm::mat4 projection = scratch::MainCamera->GetProjectionMatrix();
    glm::vec3 viewPosition =  scratch::MainCamera->getPosition();

    std::optional<scratch::Material> currentMaterial = {};
    for (auto mesh : renderQueue) {
        if (!currentMaterial.has_value() || mesh.material->ID != currentMaterial.value().ID) {
            currentMaterial = *mesh.material;
            currentMaterial.value().activate();
            currentMaterial.value().getShader()->setMat4("view", view);
            currentMaterial.value().getShader()->setMat4("projection", projection);
            currentMaterial.value().getShader()->setVec3("viewPos", viewPosition);
            directionalLight.ApplyToShader(*currentMaterial.value().getShader());
        }
        mesh.Draw();
    }
}
