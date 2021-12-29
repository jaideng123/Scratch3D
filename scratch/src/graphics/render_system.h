#pragma once

#include <camera/camera.h>

#include <utility>
#include "lights/directional_light.h"
#include "mesh.hpp"

namespace scratch {
    struct RenderItem {
        RenderItem(std::shared_ptr<scratch::Mesh> mesh, std::shared_ptr<scratch::Material> materialRef,
                   const glm::mat4 &transform) : mesh(std::move(mesh)), materialRef(std::move(materialRef)),
                                                 transform(transform) {}

        std::shared_ptr<scratch::Mesh> mesh;
        std::shared_ptr<scratch::Material> materialRef;
        glm::mat4 transform;
    };

    class RenderSystem {
    public:
        static void setup();

        static void startFrame();

        static void render(const scratch::Camera &camera, DirectionalLight directionalLight);

        static void drawMesh(const std::shared_ptr<scratch::Mesh> &mesh,
                             const std::shared_ptr<scratch::Material> &materialRef,
                             const glm::mat4 &transform);

        static void endFrame();

    private:
        static std::vector<RenderItem> _renderQueue;
    };
}
