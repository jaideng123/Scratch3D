#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include "graphics/renderable.h"
#include "graphics/model.h"

namespace scratch {
    class Entity {
    private:
        unsigned int _id;
        std::shared_ptr<scratch::Renderable> _renderable;

    public:
        Entity(const unsigned int id, std::shared_ptr<scratch::Renderable> renderable) {
            _renderable = std::move(renderable);
            _id = id;
        }

        unsigned int getID() const {
            return _id;
        }

        std::shared_ptr<scratch::Renderable> getRenderable() {
            return _renderable;
        }

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
            writer.StartObject();

            writer.String("id");
            writer.Uint(_id);
            writer.String("renderableId");
            writer.Uint(_renderable->getId());

            writer.EndObject();
        }
    };
} // namespace scratch