#pragma once

#include <memory>
#include <scene/scene_manager.h>
#include <resources/resource_manager.h>
#include <resources/shader_library.h>

namespace scratch {
    class Managers {
    public:
        Managers();
        std::unique_ptr<scratch::SceneManager> sceneManager;
        std::unique_ptr<scratch::ResourceManager> resourceManager;
        std::unique_ptr<scratch::ShaderLibrary> shaderLibrary;

    };
}

