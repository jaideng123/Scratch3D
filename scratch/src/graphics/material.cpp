//
// Created by JJJai on 12/19/2021.
//
#include <main.h>
#include "material.h"

scratch::Material::Material(unsigned int id, std::vector<std::shared_ptr<Texture>> textures) {
    _id = id;
    _textures = std::move(textures);
}

scratch::Material::Material(std::vector<std::shared_ptr<Texture>> textures) {
    _id = 0;
    _textures = std::move(textures);
}

scratch::Material::Material() {
    _id = 0;
    _textures = std::vector<std::shared_ptr<Texture>>();
}

void scratch::Material::activate() {
    _shader->use();
    setupTextures();
    setupStateParameters();
}

void scratch::Material::setShader(std::shared_ptr <scratch::Shader> shader) {
    _shader = shader;
}

std::shared_ptr <scratch::Shader> scratch::Material::getShader() {
    return _shader;
}

unsigned int scratch::Material::getId() const {
    return _id;
}

void scratch::Material::setId(unsigned int id) {
    _id = id;
}

const std::map <std::string, scratch::Parameter> &scratch::Material::getParameters() const {
    return _parameters;
}

void scratch::Material::setParameters(const std::map <std::string, scratch::Parameter> &parameters) {
    _parameters = parameters;
}

void scratch::Material::setBool(const std::string &name, bool value) {
    scratch::Parameter param;
    param.type = scratch::ParameterType::BOOL;
    param.value = value;
    _parameters[name] = param;
}

void scratch::Material::setInt(const std::string &name, int value) {
    scratch::Parameter param;
    param.type = scratch::ParameterType::INT;
    param.value = value;
    _parameters[name] = param;
}

void scratch::Material::setFloat(const std::string &name, float value) {
    scratch::Parameter param;
    param.type = scratch::ParameterType::FLOAT;
    param.value = value;
    _parameters[name] = param;
}

void scratch::Material::setMat4(const std::string &name, glm::mat4 value) {
    scratch::Parameter param;
    param.type = scratch::ParameterType::MATRIX4;
    param.value = value;
    _parameters[name] = param;
}

void scratch::Material::setVec3(const std::string &name, glm::vec3 value) {
    scratch::Parameter param;
    param.type = scratch::ParameterType::VECTOR3;
    param.value = value;
    _parameters[name] = param;
}

void scratch::Material::removeParameter(const std::string &name) {
    _parameters.erase(name);
}

void scratch::Material::renameParameter(const std::string &oldName, const std::string &newName) {
    auto nodeHandler = _parameters.extract(oldName);
    nodeHandler.key() = newName;
    _parameters.insert(std::move(nodeHandler));
}

void scratch::Material::setupTextures() {
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (auto i = 0; i < _textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::shared_ptr<scratch::Texture> texture = _textures[i];
        std::string number;
        std::string name = _textures[i]->type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(_shader->getShaderId(), ("material." + name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, _textures[i]->id);
    }
}

void scratch::Material::setupStateParameters() {
    for (auto const &[key, val] : _parameters) {
        switch (val.type) {
            case BOOL:
                _shader->setBool(key, std::any_cast<bool>(val.value));
                break;
            case INT:
                _shader->setInt(key, std::any_cast<int>(val.value));
                break;
            case FLOAT:
                _shader->setFloat(key, std::any_cast<float>(val.value));
                break;
            case VECTOR3:
                _shader->setVec3(key, std::any_cast<glm::vec3>(val.value));
                break;
            case MATRIX4:
                _shader->setMat4(key, std::any_cast<glm::mat4>(val.value));
                break;
            default:
                SCRATCH_ASSERT_NEVER("Unknown Param Type");
                break;
        }
    }
}

void scratch::Material::clearParameters() {
    for (auto const &[key, val] : _parameters) {
        switch (val.type) {
            case BOOL:
                _shader->setBool(key, false);
                break;
            case INT:
                _shader->setInt(key, 0);
                break;
            case FLOAT:
                _shader->setFloat(key, 0);
                break;
            case VECTOR3:
                _shader->setVec3(key, glm::vec3(0));
                break;
            case MATRIX4:
                _shader->setMat4(key, glm::mat4(1));
                break;
            default:
                SCRATCH_ASSERT_NEVER("Unknown Param Type");
                break;
        }
    }
}

void scratch::Material::serialize(rapidjson::PrettyWriter <rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("id");
    writer.Uint(_id);

    writer.String("shaderName");
    writer.String(_shader->getName().c_str(), static_cast<rapidjson::SizeType>(_shader->getName().length()));

    writer.String("textures");
    writer.StartArray();
    for (auto texture: _textures) {
        writer.StartObject();
        writer.String("path");
        writer.String(texture->path.c_str(), static_cast<rapidjson::SizeType>(texture->path.length()));
        writer.String("type");
        writer.String(texture->type.c_str(), static_cast<rapidjson::SizeType>(texture->type.length()));
        writer.EndObject();
    }
    writer.EndArray();


    writer.String("parameters");
    writer.StartArray();

    for (auto &param : _parameters) {
        writer.StartObject();

        writer.String("key");
        writer.String(param.first.c_str(), static_cast<rapidjson::SizeType>(param.first.length()));

        writer.String("type");
        std::string type = PARAM_TYPE_TO_STRING.find(param.second.type)->second;
        writer.String(type.c_str(), static_cast<rapidjson::SizeType>(type.length()));

        writer.String("value");
        std::string serializedValue = "";
        switch (param.second.type) {
            case BOOL:
                serializedValue = scratch::StringConverter::toString(std::any_cast<bool>(param.second.value));
                break;
            case INT:
                serializedValue = scratch::StringConverter::toString(std::any_cast<int>(param.second.value));
                break;
            case FLOAT:
                serializedValue = scratch::StringConverter::toString(std::any_cast<float>(param.second.value));
                break;
            case VECTOR3:
                serializedValue = scratch::StringConverter::toString(std::any_cast<glm::vec3>(param.second.value));
                break;
            case MATRIX4:
                serializedValue = scratch::StringConverter::toString(std::any_cast<glm::mat4>(param.second.value));
                break;
            default:
                SCRATCH_ASSERT_NEVER("Unknown Param Type");
                break;
        }
        writer.String(serializedValue.c_str(),
                      static_cast<rapidjson::SizeType>(serializedValue.length()));

        writer.EndObject();
    }

    writer.EndArray();

    writer.EndObject();
}

void scratch::Material::deserialize(const rapidjson::Value &object) {
    _id = object["id"].GetUint();

    std::string shaderName = object["shaderName"].GetString();
    _shader = ScratchManagers->shaderLibrary->findShader(shaderName);

    auto texturesArray = object["textures"].GetArray();
    for (rapidjson::Value::ConstValueIterator itr = texturesArray.Begin(); itr != texturesArray.End(); ++itr) {
        std::string textureType = (*itr)["type"].GetString();
        std::string texturePath = (*itr)["path"].GetString();
        this->addTexture(texturePath, textureType);
    }

    auto parametersArray = object["parameters"].GetArray();
    for (auto itr = parametersArray.Begin();
         itr != parametersArray.End(); ++itr) {
        std::string key = (*itr)["key"].GetString();
        std::string typeString = (*itr)["type"].GetString();
        scratch::Parameter param;
        param.type = STRING_TO_PARAM_TYPE.find(typeString)->second;
        std::string rawValue = (*itr)["value"].GetString();
        switch (param.type) {
            case BOOL:
                param.value = scratch::StringConverter::parsebool(rawValue);
                break;
            case INT:
                param.value = scratch::StringConverter::parseint(rawValue);
                break;
            case FLOAT:
                param.value = scratch::StringConverter::parsefloat(rawValue);
                break;
            case VECTOR3:
                param.value = scratch::StringConverter::parsevec3(rawValue);
                break;
            case MATRIX4:
                param.value = scratch::StringConverter::parsemat4(rawValue);
                break;
            default:
                SCRATCH_ASSERT_NEVER("Unknown Param Type");
                break;
        }
        _parameters[key] = param;
    }

}

void scratch::Material::addTexture(const std::string path, const std::string typeName) {
    _textures.push_back(ScratchManagers->resourceManager->loadTexture(path,typeName));
}

const std::string &scratch::Material::getPath() const {
    return _path;
}

void scratch::Material::setPath(const std::string &path) {
    _path = path;
}

int scratch::Material::getIndex() const {
    return _index;
}

void scratch::Material::setIndex(int index) {
    _index = index;
}
