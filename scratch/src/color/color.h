//
// Created by JJJai on 7/19/2020.
//
#pragma once

#include "glm/glm.hpp"

namespace scratch {
    class Color {
    public:
        Color(const glm::vec3 &value);

        Color() = default;

        const glm::vec3 &getValue() const;

        void setValue(const glm::vec3 &value);

    private:
        glm::vec3 _value;
    };

    static const Color WHITE = Color(glm::vec3(1.0f));
    static const Color BLACK = Color(glm::vec3(1.0f));
}