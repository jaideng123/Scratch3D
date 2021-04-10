#pragma once

#include <memory>
#include <scene/scene_manager.h>

namespace scratch {
    class Managers {
    public:
        Managers();

        std::unique_ptr<scratch::SceneManager> sceneManager;
    };
}

