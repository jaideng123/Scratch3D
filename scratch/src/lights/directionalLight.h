//
// Created by JJJai on 7/19/2020.
//
#pragma once

#include <color/color.h>
#include <shader/shader.h>
#include "glm/glm.hpp"

namespace scratch {
    class DirectionalLight {
        glm::vec3 direction;
        Color ambient;
        Color diffuse;
        Color specular;

    public:
        DirectionalLight(const glm::vec3 &direction, const Color &ambient, const Color &diffuse, const Color &specular);

        const glm::vec3 &getDirection() const;

        void setDirection(const glm::vec3 &direction);

        const Color &getAmbient() const;

        void setAmbient(const Color &ambient);

        const Color &getDiffuse() const;

        void setDiffuse(const Color &diffuse);

        const Color &getSpecular() const;

        void setSpecular(const Color &specular);

        void ApplyToShader(Shader& shader);
    };
}
