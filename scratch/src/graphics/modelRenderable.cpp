//
// Created by JJJai on 7/11/2020.
//

#include "modelRenderable.h"
#include "graphics/model.h"

scratch::ModelRenderable::ModelRenderable(scratch::Model &model) : model(model) {
    this->model = model;
}

std::vector<scratch::Mesh>& scratch::ModelRenderable::getMeshes() {
    return model.getMeshes();
}
