#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "camera/camera.h"

namespace scratch {

    class TransformGizmo {
    public:
        TransformGizmo(Camera *newCamera);

        // Draws Gizmo
        void render();

        void setCurrentTransform(glm::mat4 transform);

        glm::mat4 getCurrentTransform();

        void setCamera(scratch::Camera *newCamera);

    private:
        glm::mat4 _currentTransform{};
        scratch::Camera *_camera;
        constexpr static const float Y_OFFSET = 250.0f;
    };

}