#pragma once

// System Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <camera/camera.h>
#include <GLFW/glfw3.h>
namespace scratch {
// Define Some Constants
    const int defaultWidth = 1280;
    const int defaultHeight = 720;

    inline Camera* MainCamera = nullptr;
    inline GLFWwindow* MainWindow = nullptr;
}