#pragma once

#include <glm/glm.hpp>
#include "model/model.hpp"
namespace scratch
{
    class Entity
    {
    private:
        glm::vec3 _position;
        glm::vec3 _scale;
        scratch::Model _model;
        scratch::Shader _shader;

    public:
        Entity(glm::vec3 position, glm::vec3 scale, scratch::Model model, scratch::Shader shader)
        {
            _position = position;
            _scale = scale;
            _model = model;
            _shader = shader;
        }

        scratch::Model getModel()
        {
            return _model;
        }

        scratch::Shader getShader()
        {
            return _shader;
        }

        glm::mat4 generateTransformMatrix()
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::scale(transform, glm::vec3(.2f));
            transform = glm::translate(transform, glm::vec3(10, 0, 0));
            return transform;
        }
    };
} // namespace scratch