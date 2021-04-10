#pragma once

// System Headers
#include <GLFW/glfw3.h>
#include "camera/camera.h"
#include "managers.h"

namespace scratch {

    // Define Some Constants
    const int DEFAULT_WIDTH = 1280;
    const int DEFAULT_HEIGHT = 720;

    inline Camera *MainCamera = nullptr;
    inline GLFWwindow *MainWindow = nullptr;

    inline Managers *ScratchManagers = nullptr;
}