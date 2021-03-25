#pragma once


#include <lights/directional_light.h>
#include "mesh.hpp"

class RenderSystem {
public:
    static void setup();

    static void startFrame();

    static void render(const std::vector<scratch::Mesh> &renderQueue, scratch::DirectionalLight &directionalLight);

    static void endFrame();
};
