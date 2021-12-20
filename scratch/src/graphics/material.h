#pragma once

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <iostream>

#include <glad/glad.h>

#ifdef _MSC_VER
#undef GetObject
#endif

#include <stb_image.h>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>
#include <utilities/assert.h>
#include <any>

#include "converter/string_converter.h"
#include "shader.h"

namespace scratch {
    struct Texture {
        unsigned int id;
        std::string type;
        std::string path;
        unsigned char * data;
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
        std::any value;
        scratch::ParameterType type;
    };

    class Material {
    private:
        unsigned int _id;
        std::vector<std::shared_ptr<Texture>> _textures;
        std::shared_ptr<scratch::Shader> _shader;
        std::map<std::string, scratch::Parameter> _parameters;
    public:

        Material(unsigned int id, std::vector<std::shared_ptr<Texture>> textures);

        Material(std::vector<std::shared_ptr<Texture>> textures);

        Material();

        void activate();

        void setShader(std::shared_ptr<scratch::Shader> shader);

        std::shared_ptr<scratch::Shader> getShader();

        unsigned int getId() const;

        void setId(unsigned int id);

        const std::map<std::string, scratch::Parameter> &getParameters() const;

        void setParameters(const std::map<std::string, scratch::Parameter> &parameters);

        void setBool(const std::string &name, bool value);

        void setInt(const std::string &name, int value);

        void setFloat(const std::string &name, float value);

        void setMat4(const std::string &name, glm::mat4 value);

        void setVec3(const std::string &name, glm::vec3 value);

        void removeParameter(const std::string &name);

        void renameParameter(const std::string &oldName, const std::string &newName);

        void setupTextures();

        void setupStateParameters();

        void clearParameters();

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);

        void addTexture(const std::string path, const std::string typeName);
    };
} // namespace scratch