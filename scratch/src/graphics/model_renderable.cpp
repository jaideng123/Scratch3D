//
// Created by JJJai on 7/11/2020.
//

#include "model_renderable.h"

#include <utility>
#include "graphics/model.h"

const std::string scratch::ModelRenderable::TYPE = "MODEL";

std::vector<scratch::Mesh> &scratch::ModelRenderable::getMeshes() {
    return _model->getMeshes();
}


std::string scratch::ModelRenderable::getType() {
    return TYPE;
}

void scratch::ModelRenderable::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("type");
    writer.String(TYPE.c_str(), static_cast<rapidjson::SizeType>(TYPE.length()));
    writer.String("id");
    writer.Uint(id);
    writer.String("modelPath");
    writer.String(_model->getModelPath().c_str(),static_cast<rapidjson::SizeType>(_model->getModelPath().length()));

    writer.EndObject();
}

scratch::ModelRenderable::ModelRenderable(unsigned int id, std::shared_ptr<scratch::Model> model) : _model(
        std::move(model)) {
    this->id = id;
}

scratch::ModelRenderable::ModelRenderable() {}

const std::vector<std::shared_ptr<scratch::Material>> &scratch::ModelRenderable::getMaterials() const {
    return _model->getMaterials();
}

