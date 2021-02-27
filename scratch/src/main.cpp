// Local Headers
#include "main.h"
// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.

// System Headers
#include <GLFW/glfw3.h>

// Dear IMGUI Headers
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

//ImGuizmo Headers
#include "ImGuizmo.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <optional>

// Local Headers
#include "lights/directionalLight.h"
#include "entity/entity-factory.h"
#include "shader/shader.h"
#include "model/model.h"
#include "camera/camera.hpp"
#include "entity/entity.hpp"
#include "renderable/modelRenderable.h"


Camera camera = Camera();
std::vector<scratch::Entity> entities = std::vector<scratch::Entity>();

std::vector<scratch::Shader *> shaders = std::vector<scratch::Shader *>();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

double lastX = 400, lastY = 300;
bool firstMouse = true;

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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

bool checkSelection = false;

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

unsigned int selectedEntityId = 0;

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

void processInput(GLFWwindow *window) {
    if (ImGui::GetIO().WantCaptureMouse) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    float moveSpeed = 4.0f;
    if (selectedEntityId > 0) {
        scratch::Entity *selectedEntity = &entities[selectedEntityId - 1];
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                selectedEntity->setPosition(
                        selectedEntity->getPosition() + (glm::vec3(0, 0, 1) * deltaTime * moveSpeed));
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                selectedEntity->setPosition(
                        selectedEntity->getPosition() + (glm::vec3(0, 0, -1) * deltaTime * moveSpeed));
        } else {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                selectedEntity->setPosition(
                        selectedEntity->getPosition() + (glm::vec3(0, 1, 0) * deltaTime * moveSpeed));
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                selectedEntity->setPosition(
                        selectedEntity->getPosition() + (glm::vec3(0, -1, 0) * deltaTime * moveSpeed));
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(-1, 0, 0) * deltaTime * moveSpeed));
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            selectedEntity->setPosition(selectedEntity->getPosition() + (glm::vec3(1, 0, 0) * deltaTime * moveSpeed));
    }
}

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

void setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader &shader) {
    for (auto &mesh : meshes) {
        mesh.material->setShader(&shader);
    }
}

void editTransform(Camera &camera, glm::mat4 &matrix) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(matrix));

    if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    glm::vec3 snap = {1.f, 1.f, 1.f};
    switch (mCurrentGizmoOperation) {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap.x);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap.x);
            break;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()), glm::value_ptr(camera.GetProjectionMatrix()),
                         mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix), NULL,
                         useSnap ? &snap.x : NULL);
}

//TODO: Render to separate frame buffer
void handleSelection(scratch::Shader &selectionShader, glm::mat4 &view, glm::mat4 &projection) {
    glDisable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glReadPixels(lastX, mHeight - lastY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);
    printf("%u %u %u\n", pixel[0], pixel[1], pixel[2]);
    selectedEntityId = pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main() {
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
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Contex");
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
    glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
    glViewport(0, 0, mWidth, mHeight);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    const char *glsl_version = "#version 400 core";
    ImGui_ImplOpenGL3_Init(glsl_version);

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
            entityFactory.create_entity(glm::vec3(0, 5, 0), glm::vec3(0.2f),
                                        new scratch::ModelRenderable(nanosuitModel)));
    entities.push_back(
            entityFactory.create_entity(glm::vec3(0, 0, 0), glm::vec3(0.2f), new scratch::ModelRenderable(stoneModel)));

    selectedEntityId = 0;

    scratch::DirectionalLight directionalLight = scratch::DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
                                                                           scratch::Color(glm::vec3(0.2f)),
                                                                           scratch::Color(glm::vec3(0.5f)),
                                                                           scratch::WHITE);

    glEnable(GL_DEPTH_TEST);

    // Enable Gamma correction (physically correct colors)
    glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);

    std::cout << "starting rendering loop" << std::endl;
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
//        bool showDemoWindow = true;
//        ImGui::ShowDemoWindow(&showDemoWindow);
        ImGuizmo::BeginFrame();

        if (selectedEntityId != 0) {
            glm::mat4 matrix = entities[selectedEntityId - 1].generateTransformMatrix();
            editTransform(camera, matrix);
        }


        ImGui::Render();
        // Background Fill Color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();

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

        if (checkSelection) {
            handleSelection(selectionShader, view, projection);
            checkSelection = false;
        }

        std::optional<scratch::Material> currentMaterial = {};
        for (auto mesh : renderQueue) {
            if (!currentMaterial.has_value() || mesh.material->ID != currentMaterial.value().ID) {
                currentMaterial = *mesh.material;
                currentMaterial.value().activate();
                currentMaterial.value().getShader()->setMat4("view", view);
                currentMaterial.value().getShader()->setMat4("projection", projection);
                currentMaterial.value().getShader()->setVec3("viewPos", camera.getPosition());
                directionalLight.ApplyToShader(*currentMaterial.value().getShader());
            }
            mesh.Draw();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        processInput(mWindow);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
