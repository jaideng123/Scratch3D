//
// Created by JJJai on 7/19/2020.
//
#pragma once

namespace scratch { class Shader; }

#include "color/color.h"
#include "glm/glm.hpp"
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>

namespace scratch {
    class DirectionalLight {
    public:
        DirectionalLight(const glm::vec3 &_direction, const scratch::Color &_ambient, const scratch::Color &_diffuse,
                         const scratch::Color &_specular);

        DirectionalLight() {}

        const glm::vec3 &getDirection() const;

        void setDirection(const glm::vec3 &_direction);

        const scratch::Color &getAmbient() const;

        void setAmbient(const scratch::Color &_ambient);

        const scratch::Color &getDiffuse() const;

        void setDiffuse(const scratch::Color &_diffuse);

        const scratch::Color &getSpecular() const;

        void setSpecular(const scratch::Color &_specular);

        void applyToShader(Shader &shader);

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);

    private:
        glm::vec3 _direction;
        scratch::Color _ambient;
        scratch::Color _diffuse;
        scratch::Color _specular;
    };
}
