#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include "model/model.hpp"
namespace scratch
{
    class Entity
    {
    private:
        unsigned int _ID;
        glm::vec3 _position;
        glm::vec3 _scale;
        scratch::Model _model;

    public:
        Entity(glm::vec3 position, glm::vec3 scale, scratch::Model model)
        {
            _position = position;
            _scale = scale;
            _model = model;
            _ID = rand();
        }

        unsigned int getID()
        {
            return _ID;
        }

        scratch::Model getModel()
        {
            return _model;
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