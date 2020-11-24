//
// Created by JJJai on 7/19/2020.
//
#pragma once

namespace scratch {class Shader;}
#include "color/color.h"
#include "glm/glm.hpp"

namespace scratch {
    class DirectionalLight {
        glm::vec3 direction;
        scratch::Color ambient;
        scratch::Color diffuse;
        scratch::Color specular;

    public:
        DirectionalLight(const glm::vec3 &direction, const scratch::Color &ambient, const scratch::Color &diffuse, const scratch::Color &specular);

        const glm::vec3 &getDirection() const;

        void setDirection(const glm::vec3 &direction);

        const scratch::Color &getAmbient() const;

        void setAmbient(const scratch::Color &ambient);

        const scratch::Color &getDiffuse() const;

        void setDiffuse(const scratch::Color &diffuse);

        const scratch::Color &getSpecular() const;

        void setSpecular(const scratch::Color &specular);

        void ApplyToShader(Shader& shader);
    };
}
