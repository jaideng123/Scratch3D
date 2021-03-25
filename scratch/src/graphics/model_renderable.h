#pragma once

#include <vector>
#include <memory>
#include "renderable.h"

namespace scratch { class Model; }
namespace scratch { class Mesh; }

namespace scratch {
    class ModelRenderable : public scratch::Renderable {
    public:
        ModelRenderable(unsigned int id, std::shared_ptr<scratch::Model> model);

        ModelRenderable();

        std::vector<scratch::Mesh> &getMeshes() override;

        static const std::string TYPE;

        std::string getType() override;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;

    private:
        std::shared_ptr<scratch::Model> _model;
    };
}