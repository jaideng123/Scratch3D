//
// Created by JJJai on 7/19/2020.
//

#include "directionalLight.h"
#include "graphics/shader.h"

scratch::DirectionalLight::DirectionalLight(const glm::vec3 &direction, const scratch::Color &ambient,
                                            const scratch::Color &diffuse, const scratch::Color &specular) : direction(
        direction), ambient(ambient), diffuse(diffuse), specular(specular) {}

const glm::vec3 &scratch::DirectionalLight::getDirection() const {
    return direction;
}

void scratch::DirectionalLight::setDirection(const glm::vec3 &direction) {
    DirectionalLight::direction = direction;
}

const scratch::Color &scratch::DirectionalLight::getAmbient() const {
    return ambient;
}

void scratch::DirectionalLight::setAmbient(const scratch::Color &ambient) {
    DirectionalLight::ambient = ambient;
}

const scratch::Color &scratch::DirectionalLight::getDiffuse() const {
    return diffuse;
}

void scratch::DirectionalLight::setDiffuse(const scratch::Color &diffuse) {
    DirectionalLight::diffuse = diffuse;
}

const scratch::Color &scratch::DirectionalLight::getSpecular() const {
    return specular;
}

void scratch::DirectionalLight::setSpecular(const scratch::Color &specular) {
    DirectionalLight::specular = specular;
}

void scratch::DirectionalLight::ApplyToShader(scratch::Shader &shader) {
    shader.setVec3("dirLight.direction", direction);
    shader.setVec3("dirLight.ambient", ambient.getValue());
    shader.setVec3("dirLight.diffuse", diffuse.getValue());
    shader.setVec3("dirLight.specular", specular.getValue());
}
