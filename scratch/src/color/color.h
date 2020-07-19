//
// Created by JJJai on 7/19/2020.
//
#pragma once

#include "glm/glm.hpp"
namespace scratch {
    class Color {
        glm::vec3 value;
    public:
        Color(const glm::vec3 &value);
        Color() = default;

        const glm::vec3 & getValue() const;
        void setValue(const glm::vec3 &value);
    };
    static const Color WHITE = Color(glm::vec3(1.0f));
    static const Color BLACK = Color(glm::vec3(1.0f));
}