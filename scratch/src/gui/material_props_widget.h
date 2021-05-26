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

        std::string renderPropertyName(const std::shared_ptr<Material> &material, const std::string& propertyName) const;
    };

}