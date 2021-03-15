//
// Created by JJJai on 7/19/2020.
//

#include "directionalLight.h"
#include "graphics/shader.h"

scratch::DirectionalLight::DirectionalLight(const glm::vec3 &direction, const scratch::Color &ambient,
                                            const scratch::Color &diffuse, const scratch::Color &specular) : _direction(
        direction), _ambient(ambient), _diffuse(diffuse), _specular(specular) {}

const glm::vec3 &scratch::DirectionalLight::getDirection() const {
    return _direction;
}

void scratch::DirectionalLight::setDirection(const glm::vec3 &direction) {
    _direction = direction;
}

const scratch::Color &scratch::DirectionalLight::getAmbient() const {
    return _ambient;
}

void scratch::DirectionalLight::setAmbient(const scratch::Color &ambient) {
    _ambient = ambient;
}

const scratch::Color &scratch::DirectionalLight::getDiffuse() const {
    return _diffuse;
}

void scratch::DirectionalLight::setDiffuse(const scratch::Color &diffuse) {
    _diffuse = diffuse;
}

const scratch::Color &scratch::DirectionalLight::getSpecular() const {
    return _specular;
}

void scratch::DirectionalLight::setSpecular(const scratch::Color &specular) {
    _specular = specular;
}

void scratch::DirectionalLight::ApplyToShader(scratch::Shader &shader) {
    shader.setVec3("dirLight.direction", _direction);
    shader.setVec3("dirLight.ambient", _ambient.getValue());
    shader.setVec3("dirLight.diffuse", _diffuse.getValue());
    shader.setVec3("dirLight.specular", _specular.getValue());
}
