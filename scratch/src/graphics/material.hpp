#pragma once

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h> // holds all OpenGL type declarations

#include "converter/stringConverter.h"
#include "shader.h"

namespace scratch {
    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
    };

    enum ParameterType {
        BOOL,
        INT,
        FLOAT,
        VECTOR3,
        MATRIX4
    };
    const std::map<ParameterType, std::string> PARAM_TYPE_TO_STRING{{BOOL,    "BOOL"},
                                                                    {INT,     "INT"},
                                                                    {FLOAT,   "FLOAT"},
                                                                    {VECTOR3, "VECTOR3"},
                                                                    {MATRIX4, "MATRIX4"}};
    const std::map<std::string, ParameterType> STRING_TO_PARAM_TYPE{{"BOOL",    BOOL},
                                                                    {"INT",     INT},
                                                                    {"FLOAT",   FLOAT},
                                                                    {"VECTOR3", VECTOR3},
                                                                    {"MATRIX4", MATRIX4}};

    struct Parameter {
        std::string value;
        scratch::ParameterType type;
    };

    class Material {
    public:
        unsigned int Id;
        std::map<std::string, scratch::Parameter> parameters;

        Material(std::vector<Texture> textures) {
            Id = rand();
            _textures = textures;
        }

        Material() = default;

        void activate() {
            _shader->use();
            setupTextures();
            setStateParameters();
        }

        void setShader(std::shared_ptr<scratch::Shader> shader) {
            _shader = shader;
        }

        std::shared_ptr<scratch::Shader> getShader() {
            return _shader;
        }

        void setBool(const std::string &name, bool value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::BOOL;
            param.value = scratch::StringConverter::toString(value, false);
            parameters[name] = param;
        }

        void setInt(const std::string &name, int value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::INT;
            param.value = scratch::StringConverter::toString(value);
            parameters[name] = param;
        }

        void setFloat(const std::string &name, float value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::FLOAT;
            param.value = scratch::StringConverter::toString(value);
            parameters[name] = param;
        }

        void setMat4(const std::string &name, glm::mat4 value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::MATRIX4;
            param.value = scratch::StringConverter::toString(value);
            parameters[name] = param;
        }

        glm::mat4 getMat4(const std::string &name) {
            return scratch::StringConverter::parsemat4(parameters.find(name)->second.value);
        }


        void setVec3(const std::string &name, glm::vec3 value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::VECTOR3;
            param.value = scratch::StringConverter::toString(value);
            parameters[name] = param;
        }

        void setupTextures() {
            // bind appropriate textures
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = 0; i < _textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                scratch::Texture texture = _textures[i];
                std::string number;
                std::string name = _textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to stream
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to stream
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to stream

                // now set the sampler to the correct texture unit
                glUniform1i(glGetUniformLocation(_shader->shaderId, ("material." + name + number).c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, _textures[i].id);
            }
        }

        void setStateParameters() {
            for (auto const &[key, val] : parameters) {
                switch (val.type) {
                    case BOOL:
                        _shader->setBool(key, scratch::StringConverter::parsebool(val.value));
                        break;
                    case INT:
                        _shader->setInt(key, scratch::StringConverter::parseint(val.value));
                        break;
                    case FLOAT:
                        _shader->setFloat(key, scratch::StringConverter::parsefloat(val.value));
                        break;
                    case VECTOR3:
                        _shader->setVec3(key, scratch::StringConverter::parsevec3(val.value));
                        break;
                    case MATRIX4:
                        _shader->setMat4(key, scratch::StringConverter::parsemat4(val.value));
                        break;
                    default:
                        throw std::runtime_error("UNKNOWN DATA TYPE");
                        break;
                }
            }
        }

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
            writer.StartObject();

            writer.String("textures");
            writer.StartArray();
            for (scratch::Texture texture: _textures) {
                writer.StartObject();
                writer.String("path");
                writer.String(texture.path.c_str(), static_cast<rapidjson::SizeType>(texture.path.length()));
                writer.String("type");
                writer.String(texture.type.c_str(), static_cast<rapidjson::SizeType>(texture.type.length()));
                writer.EndObject();
            }
            writer.EndArray();


            writer.String("parameters");
            writer.StartObject();
            // Iterate over the map using c++11 range based for loop
            for (const std::pair<std::string, scratch::Parameter> &param : parameters) {
                // TODO: remove once this is no longer a material property
                if (param.first == "model") {
                    continue;
                }
                writer.String(param.first.c_str(), static_cast<rapidjson::SizeType>(param.first.length()));
                writer.StartObject();
                writer.String("type");
                std::string type = PARAM_TYPE_TO_STRING.find(param.second.type)->second;
                writer.String(type.c_str(), static_cast<rapidjson::SizeType>(type.length()));
                writer.String("value");
                writer.String(param.second.value.c_str(),
                              static_cast<rapidjson::SizeType>(param.second.value.length()));
                writer.EndObject();
            }
            writer.EndObject();

            writer.EndObject();
        }

    private:
        std::vector<Texture> _textures;
        std::shared_ptr<scratch::Shader> _shader;
    };
} // namespace scratch