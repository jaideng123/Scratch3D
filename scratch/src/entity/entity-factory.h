//
// Created by JJJai on 12/12/2020.
//

#ifndef SCRATCH_ENTITY_FACTORY_H
#define SCRATCH_ENTITY_FACTORY_H

#include "entity.hpp"

namespace scratch {

    class EntityFactory {
    private:
        const unsigned int MAX_ID = (255) + (255 << 8) + (255 << 16);
        unsigned int _lastGeneratedId;

        unsigned int generate_id();

    public:
        EntityFactory();

        scratch::Entity create_entity(glm::vec3 position, glm::vec3 scale, scratch::Renderable *renderable);
    };
}


#endif //SCRATCH_ENTITY_FACTORY_H
