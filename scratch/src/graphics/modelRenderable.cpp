//
// Created by JJJai on 7/11/2020.
//

#include "modelRenderable.h"

#include <utility>
#include "graphics/model.h"

std::vector<scratch::Mesh> &scratch::ModelRenderable::getMeshes() {
    return model->getMeshes();
}


std::string scratch::ModelRenderable::getType() {
    return TYPE;
}

void scratch::ModelRenderable::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("type");
    writer.String(TYPE.c_str(), static_cast<rapidjson::SizeType>(TYPE.length()));
    writer.String("id");
    writer.Uint(Id);
    writer.String("modelId");
    writer.Uint(model->Id);

    writer.EndObject();
}

scratch::ModelRenderable::ModelRenderable(unsigned int Id, std::shared_ptr<scratch::Model> model) : model(
        std::move(model)) {
    this->Id = Id;
}


