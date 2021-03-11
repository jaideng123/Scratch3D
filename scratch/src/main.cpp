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
#include <graphics/render_system.h>

// Local Headers
#include "lights/directionalLight.h"
#include "entity/entity-factory.h"
#include "camera/camera.h"
#include "graphics/modelRenderable.h"
#include "scene/scene.h"

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

    scratch::MainCamera->ProcessMouseMovement(xoffset, yoffset);
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

void processInput() {
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
            scratch::MainCamera->ProcessKeyboard(scratch::FORWARD, deltaTime);
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_S) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::BACKWARD, deltaTime);
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_A) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::LEFT, deltaTime);
        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_D) == GLFW_PRESS)
            scratch::MainCamera->ProcessKeyboard(scratch::RIGHT, deltaTime);
    } else {
        glfwSetInputMode(scratch::MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void setDefaultShader(std::vector<scratch::Mesh> &meshes, scratch::Shader &shader) {
    for (auto &mesh : meshes) {
        mesh.material->setShader(&shader);
    }
}

glm::mat4 editTransform(scratch::Camera &camera, glm::mat4 &matrix) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
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
    float *matrixPointer = glm::value_ptr(matrix);
    ImGuizmo::DecomposeMatrixToComponents(matrixPointer, matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrixPointer);

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
    glm::vec3 snap = {1.0f, 1.0f, 1.0f};
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
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
    ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()), glm::value_ptr(camera.GetProjectionMatrix()),
                         mCurrentGizmoOperation, mCurrentGizmoMode, matrixPointer, NULL,
                         useSnap ? &snap.x : NULL);
    return glm::make_mat4(matrixPointer);
}

//TODO: Render to separate frame buffer
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
    scratch::MainWindow = glfwCreateWindow(scratch::defaultWidth, scratch::defaultHeight, "Scratch", nullptr, nullptr);

    // Check for Valid Context
    if (scratch::MainWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(scratch::MainWindow);

    // hide + capture cursor
    // get cursor input
    glfwSetCursorPosCallback(scratch::MainWindow, mouse_callback);
    glfwSetKeyCallback(scratch::MainWindow, keyCallback);
    glfwSetMouseButtonCallback(scratch::MainWindow, mouse_button_callback);

    RenderSystem::setup();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(scratch::MainWindow, true);
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
            entityFactory.create_entity(glm::vec3(0, 0, 0), glm::vec3(0.2f),
                                        new scratch::ModelRenderable(stoneModel)));

    selectedEntityId = 0;

    scratch::DirectionalLight directionalLight = scratch::DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f),
                                                                           scratch::Color(glm::vec3(0.2f)),
                                                                           scratch::Color(glm::vec3(0.5f)),
                                                                           scratch::WHITE);

    scratch::MainCamera = new scratch::Camera();

    std::cout << "starting rendering loop" << std::endl;
    // Rendering Loop
    while (glfwWindowShouldClose(scratch::MainWindow) == false) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
//        bool showDemoWindow = true;
//        ImGui::ShowDemoWindow(&showDemoWindow);
        ImGuizmo::BeginFrame();

        // Edit Transform
        if (selectedEntityId != 0) {
            glm::mat4 matrix = entities[selectedEntityId - 1].generateTransformMatrix();
            entities[selectedEntityId - 1].setTransform(editTransform(*scratch::MainCamera, matrix));
        }

        ImGui::Render();

        if (glfwGetKey(scratch::MainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(scratch::MainWindow, true);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        processInput();

        // Flip Buffers and Draw
        glfwSwapBuffers(scratch::MainWindow);
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}