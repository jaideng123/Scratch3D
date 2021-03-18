//
// Created by JJJai on 7/11/2020.
//

#ifndef SCRATCH_MODELRENDERABLE_H
#define SCRATCH_MODELRENDERABLE_H


#include <vector>
#include <memory>
#include "renderable.h"

namespace scratch {class Model;}
namespace scratch {class Mesh;}

namespace scratch {
    class ModelRenderable : public scratch::Renderable {
    public:
        ModelRenderable(unsigned int Id, std::shared_ptr<scratch::Model> model);

        std::vector<scratch::Mesh> &getMeshes() override;

    private:
        std::shared_ptr<scratch::Model> model;
    public:
        const std::string TYPE = "MODEL";
        std::string getType() override;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) override;
    };
}


#endif //SCRATCH_MODELRENDERABLE_H
