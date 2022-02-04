#pragma once

#include <vector>
#include <memory>
#include <entity/entity_component.h>
#include "material.h"

namespace scratch { class Model; }
namespace scratch { class Mesh; }

namespace scratch {
    class ModelRenderable : public scratch::EntityComponent {
    public:
        ModelRenderable(const std::shared_ptr<scratch::Model> &model);

        ModelRenderable(std::shared_ptr<scratch::Model> model,
                        const std::vector<std::shared_ptr<scratch::Material>> &materials);

        ModelRenderable();

        std::vector<std::shared_ptr<scratch::Mesh>> &getMeshes();

        static const std::string TYPE_NAME;

        std::string getTypeName() override;

        const std::vector<std::shared_ptr<scratch::Material>> &getMaterials() const;


        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;

    private:
        std::shared_ptr<scratch::Model> _model;
        std::vector<std::shared_ptr<scratch::Material>> _materials;
    };
}