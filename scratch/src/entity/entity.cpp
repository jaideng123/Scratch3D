//
// Created by JJJai on 1/1/2022.
//

#include "entity.h"

void scratch::Entity::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("id");
    writer.Uint(_id);
    writer.String("components");
    writer.StartArray();
    for (const auto &component: _components) {
        component->serialize(writer);
    }
    writer.EndArray();

    writer.EndObject();
}

scratch::Entity::Entity(unsigned int id, scratch::Scene *scene) : _id(id), _sceneRef(scene) {}
