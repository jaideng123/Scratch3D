#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <renderable/renderable.h>
#include "model/model.h"

namespace scratch {
    class Entity {
    private:
        unsigned int _id;
        glm::vec3 _position;
        glm::vec3 _scale;
        glm::quat _rotation;
        scratch::Renderable *_renderable;

    public:
        Entity(const glm::vec3 position,
               const glm::vec3 scale,
               scratch::Renderable *renderable,
               const unsigned int id) {
            _position = position;
            _scale = scale;
            _rotation = glm::quat();
            _renderable = renderable;
            _id = id;
        }

        unsigned int getID() {
            return _id;
        }

        scratch::Renderable *getRenderable() {
            return _renderable;
        }

        glm::mat4 generateTransformMatrix() {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), _scale);
            glm::mat4 rotation = glm::mat4_cast(_rotation);
            return translation * rotation * scale;
        }

        void setTransform(glm::mat4 transform) {
            glm::vec3 skew = glm::vec3();
            glm::vec4 perspective = glm::vec4();
            glm::decompose(transform, _scale, _rotation, _position, skew, perspective);
        }

        glm::vec3 getPosition() {
            return _position;
        }

        void setPosition(glm::vec3 position) {
            _position = position;
        }

        glm::vec3 getScale() {
            return _scale;
        }

        void setScale(glm::vec3 scale) {
            _scale = scale;
        }

        glm::quat getRotation() {
            return _rotation;
        }

        void setRotation(glm::quat rotation) {
            _rotation = rotation;
        }
    };
} // namespace scratch