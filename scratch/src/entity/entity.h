#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include "graphics/renderable.h"
#include "graphics/model.h"

namespace scratch {
    class Entity {
    public:
        Entity(const unsigned int id, std::shared_ptr<scratch::Renderable> renderable);

        unsigned int getID() const {
            return _id;
        }

        std::shared_ptr<scratch::Renderable> getRenderable();

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);
    private:
        unsigned int _id;
        std::shared_ptr<scratch::Renderable> _renderable;
    };
}
