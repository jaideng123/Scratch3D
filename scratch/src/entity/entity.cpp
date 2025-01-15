//
// Created by JJJai on 1/1/2022.
//

#include "entity.h"

std::shared_ptr<scratch::Renderable> scratch::Entity::getRenderable() {
    return _renderable;
}

scratch::Entity::Entity(const unsigned int id, std::shared_ptr<scratch::Renderable> renderable) {
    _renderable = std::move(renderable);
    _id = id;
}

void scratch::Entity::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("id");
    writer.Uint(_id);
    writer.String("renderableId");
    unsigned int renderableId = _renderable->getId();
    writer.Uint(renderableId);

    writer.EndObject();
}
