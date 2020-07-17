//
// Created by JJJai on 7/11/2020.
//

#ifndef SCRATCH_MODELRENDERABLE_H
#define SCRATCH_MODELRENDERABLE_H


#include "model/model.h"
#include "renderable.h"

namespace scratch {
    class ModelRenderable : public Renderable {
    public:
        ModelRenderable(scratch::Model &model);

        std::vector<scratch::Mesh> &getMeshes() override;

    private:
        scratch::Model &model;
    };
}


#endif //SCRATCH_MODELRENDERABLE_H
