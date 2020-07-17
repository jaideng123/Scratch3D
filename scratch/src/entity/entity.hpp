#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include <renderable/renderable.h>
#include "model/model.h"

namespace scratch
{
    class Entity
    {
    private:
        unsigned int _ID;
        glm::vec3 _position;
        glm::vec3 _scale;
        scratch::Renderable *_renderable;

    public:
        Entity(const glm::vec3 position, const glm::vec3 scale, scratch::Renderable *renderable)
        {
            _position = position;
            _scale = scale;
            _renderable = renderable;
            _ID = rand();
        }

        unsigned int getID()
        {
            return _ID;
        }

        scratch::Renderable* getRenderable()
        {
            return _renderable;
        }

        glm::mat4 generateTransformMatrix()
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::scale(transform, _scale);
            transform = glm::translate(transform, _position);
            return transform;
        }

        glm::vec3 getPosition()
        {
            return _position;
        }
        void setPosition(glm::vec3 position)
        {
            _position = position;
        }
    };
} // namespace scratch