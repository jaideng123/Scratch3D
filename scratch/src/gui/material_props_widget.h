#pragma once

#include <vector>
#include <memory>
#include "graphics/material.hpp"

namespace scratch {

    class MaterialPropsWidget {
    public:
        void render();

        void setMaterials(const std::vector<std::shared_ptr<Material>> &materials);

    private:
        std::vector<std::shared_ptr<Material>> _materials;
    };

}