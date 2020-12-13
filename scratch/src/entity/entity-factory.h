//
// Created by JJJai on 12/12/2020.
//

#ifndef SCRATCH_ENTITY_FACTORY_H
#define SCRATCH_ENTITY_FACTORY_H

#include "entity.hpp"

namespace scratch {

    class EntityFactory {
    private:
        // TODO: increase this once I figure out how to encode higher ints
        const unsigned int MAX_ID = 255;
        unsigned int _lastGeneratedId;
        unsigned int generate_id();

    public:
        EntityFactory();
        scratch::Entity create_entity(const glm::vec3 position, const glm::vec3 scale, scratch::Renderable *renderable);
    };
}


#endif //SCRATCH_ENTITY_FACTORY_H
