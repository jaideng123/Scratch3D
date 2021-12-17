//
// Created by JJJai on 4/10/2021.
//

#include "managers.h"

scratch::Managers::Managers() {
    sceneManager = std::make_unique<SceneManager>();
    resourceManager = std::make_unique<ResourceManager>();
}
