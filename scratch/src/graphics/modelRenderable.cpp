//
// Created by JJJai on 7/11/2020.
//

#include "modelRenderable.h"
#include "graphics/model.h"

std::vector<scratch::Mesh>& scratch::ModelRenderable::getMeshes() {
    return model->getMeshes();
}

scratch::ModelRenderable::ModelRenderable(std::shared_ptr<scratch::Model> model) : model(model) {}
