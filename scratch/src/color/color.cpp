//
// Created by JJJai on 7/19/2020.
//

#include "color.h"

scratch::Color::Color(const glm::vec3 &value) : _value(value) {}

const glm::vec3 &scratch::Color::getValue() const {
    return _value;
}

void scratch::Color::setValue(const glm::vec3 &value) {
    Color::_value = value;
}

