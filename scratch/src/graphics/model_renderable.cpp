//
// Created by JJJai on 7/11/2020.
//

#include "model_renderable.h"

#include <utility>
#include "graphics/model.h"

const std::string scratch::ModelRenderable::TYPE = "MODEL";

std::vector<std::shared_ptr<scratch::Mesh>> &scratch::ModelRenderable::getMeshes() {
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
    writer.Uint(_id);
    writer.String("modelPath");
    writer.String(_model->getModelPath().c_str(), static_cast<rapidjson::SizeType>(_model->getModelPath().length()));
    writer.String("materials");
    writer.StartArray();
    for (auto material: getMaterials()) {
        material->serialize(writer);
    }
    writer.EndArray();

    writer.EndObject();
}

scratch::ModelRenderable::ModelRenderable(unsigned int id, std::shared_ptr<scratch::Model> model) : ModelRenderable(id,
                                                                                                                    model,
                                                                                                                    model->getDefaultMaterials()) {}

scratch::ModelRenderable::ModelRenderable(unsigned int id, std::shared_ptr<scratch::Model> model,
                                          std::vector<std::shared_ptr<scratch::Material>> materials) : _model(model) {
    _materials.clear();
    for (const auto& material: model->getDefaultMaterials()) {
        _materials.emplace_back(std::make_shared<scratch::Material>(*material));
    }
    this->_id = id;
}

scratch::ModelRenderable::ModelRenderable() {}

const std::vector<std::shared_ptr<scratch::Material>> &scratch::ModelRenderable::getMaterials() const {
    return _materials;
}



