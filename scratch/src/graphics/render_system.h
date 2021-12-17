#pragma once

#include "lights/directional_light.h"
#include "mesh.hpp"
namespace scratch {
    struct RenderItem {
        RenderItem(const scratch::Mesh &mesh, const std::shared_ptr<scratch::Material> &materialRef,
                   const glm::mat4 &transform) : mesh(mesh), materialRef(materialRef),
                                                 transform(transform) {}
        scratch::Mesh mesh;
        std::shared_ptr<scratch::Material> materialRef;
        glm::mat4 transform;
    };

    class RenderSystem {
    public:
        static void setup();

        static void startFrame();

        static void render(const std::vector<RenderItem> &renderQueue, scratch::DirectionalLight &directionalLight);

        static void endFrame();
    };
}
