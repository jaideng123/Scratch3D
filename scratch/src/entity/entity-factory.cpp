//
// Created by JJJai on 12/12/2020.
//

#include "entity-factory.h"

scratch::EntityFactory::EntityFactory() {
    _lastGeneratedId = 0;
}

scratch::Entity scratch::EntityFactory::create_entity(const glm::vec3 position, const glm::vec3 scale,
                                                      scratch::Renderable *renderable) {
    return scratch::Entity(position, scale, renderable, generate_id());
}

unsigned int scratch::EntityFactory::generate_id() {
    unsigned int new_id = this->_lastGeneratedId + 1;
    _lastGeneratedId = new_id;
    if (new_id > this->MAX_ID) {
        throw std::runtime_error("Max ID Reached");
    }
    return new_id;
}
