#pragma once

#include <string>
#include <utility>
#include <vector>
#include <map>

#include <glad/glad.h>

#ifdef _MSC_VER
#undef GetObject
#endif

#include <stb_image.h>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>

#include "converter/string_converter.h"
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
    private:
        unsigned int _id;
        std::vector<Texture> _textures;
        std::shared_ptr<scratch::Shader> _shader;
        std::map<std::string, scratch::Parameter> _parameters;
    public:

        Material(unsigned int id, std::vector<Texture> textures) {
            _id = id;
            _textures = std::move(textures);
        }

        Material(std::vector<Texture> textures) {
            _id = 0;
            _textures = std::move(textures);
        }

        Material() {
            _textures = std::vector<scratch::Texture>();
        }

        void activate() {
            _shader->use();
            setupTextures();
            setupStateParameters();
        }

        void setShader(std::shared_ptr<scratch::Shader> shader) {
            _shader = shader;
        }

        std::shared_ptr<scratch::Shader> getShader() {
            return _shader;
        }

        unsigned int getId() const {
            return _id;
        }

        void setId(unsigned int id) {
            _id = id;
        }

        const std::map<std::string, scratch::Parameter> &getParameters() const {
            return _parameters;
        }

        void setParameters(const std::map<std::string, scratch::Parameter> &parameters) {
            _parameters = parameters;
        }

        void setBool(const std::string &name, bool value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::BOOL;
            param.value = scratch::StringConverter::toString(value, false);
            _parameters[name] = param;
        }

        void setInt(const std::string &name, int value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::INT;
            param.value = scratch::StringConverter::toString(value);
            _parameters[name] = param;
        }

        void setFloat(const std::string &name, float value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::FLOAT;
            param.value = scratch::StringConverter::toString(value);
            _parameters[name] = param;
        }

        void setMat4(const std::string &name, glm::mat4 value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::MATRIX4;
            param.value = scratch::StringConverter::toString(value);
            _parameters[name] = param;
        }

        void setVec3(const std::string &name, glm::vec3 value) {
            scratch::Parameter param;
            param.type = scratch::ParameterType::VECTOR3;
            param.value = scratch::StringConverter::toString(value);
            _parameters[name] = param;
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
                glUniform1i(glGetUniformLocation(_shader->getShaderId(), ("material." + name + number).c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, _textures[i].id);
            }
        }

        void setupStateParameters() {
            for (auto const &[key, val] : _parameters) {
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

            writer.String("id");
            writer.Uint(_id);

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

            for (const std::pair<std::string, scratch::Parameter> &param : _parameters) {
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

        void deserialize(const rapidjson::Value &object) {
            _id = object["id"].GetUint();

            auto texturesArray = object["textures"].GetArray();
            for (rapidjson::Value::ConstValueIterator itr = texturesArray.Begin(); itr != texturesArray.End(); ++itr) {
                std::string textureType = (*itr)["type"].GetString();
                std::string texturePath = (*itr)["path"].GetString();
                this->addTexture(texturePath, textureType);
            }
// TODO: work around this
//            auto parameterMap = object["parameters"].GetObject();
//            for (auto itr = parameterMap.MemberBegin();
//                 itr != parameterMap.MemberEnd(); ++itr) {
//                std::string key = (*itr).name.GetString();
//                std::string typeString = (*itr).value.GetObject()["type"].GetString();
//                scratch::Parameter param;
//                param.type = STRING_TO_PARAM_TYPE.find(typeString)->second;
//                param.value = (*itr).value.GetObject()["value"].GetString();
//                _parameters[key] = param;
//            }

        }


        void addTexture(const std::string path, const std::string typeName) {
            Texture texture;
            texture.id = textureFromFile(path);
            texture.type = typeName;
            texture.path = path;
            _textures.push_back(texture);
        }

        // TODO: move to TextureManager/ResourceManager
        unsigned int textureFromFile(const std::string &path, bool gamma = false) {
            return textureFromFile(path, gamma, GL_REPEAT);
        }

        unsigned int textureFromFile(const std::string &path, bool gamma, int wrapMode) {
            std::string filename = std::string(path);

            unsigned int textureId;
            glGenTextures(1, &textureId);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data) {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureId);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            } else {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureId;
        }
    };
} // namespace scratch