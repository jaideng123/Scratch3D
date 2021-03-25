//
// Created by JJJai on 7/19/2020.
//

#include <converter/string_converter.h>
#include "directional_light.h"
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

void scratch::DirectionalLight::applyToShader(scratch::Shader &shader) {
    shader.setVec3("dirLight.direction", _direction);
    shader.setVec3("dirLight.ambient", _ambient.getValue());
    shader.setVec3("dirLight.diffuse", _diffuse.getValue());
    shader.setVec3("dirLight.specular", _specular.getValue());
}

void scratch::DirectionalLight::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("direction");
    std::string serializedDirection = scratch::StringConverter::toString(_direction);
    writer.String(serializedDirection.c_str(), static_cast<rapidjson::SizeType>(serializedDirection.length()));

    writer.String("ambient");
    std::string serializedAmbient = scratch::StringConverter::toString(_ambient.getValue());
    writer.String(serializedAmbient.c_str(), static_cast<rapidjson::SizeType>(serializedAmbient.length()));

    writer.String("diffuse");
    std::string serializedDiffuse = scratch::StringConverter::toString(_diffuse.getValue());
    writer.String(serializedDiffuse.c_str(), static_cast<rapidjson::SizeType>(serializedDiffuse.length()));

    writer.String("specular");
    std::string serializedSpecular = scratch::StringConverter::toString(_specular.getValue());
    writer.String(serializedSpecular.c_str(), static_cast<rapidjson::SizeType>(serializedSpecular.length()));

    writer.EndObject();
}

void scratch::DirectionalLight::deserialize(const rapidjson::Value &object) {
    _direction = scratch::StringConverter::parsevec3(object["direction"].GetString());
    _ambient = scratch::StringConverter::parsevec3(object["ambient"].GetString());
    _diffuse = scratch::StringConverter::parsevec3(object["diffuse"].GetString());
    _specular = scratch::StringConverter::parsevec3(object["specular"].GetString());
}
