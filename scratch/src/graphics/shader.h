#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <include/rapidjson/writer.h>
#include <include/rapidjson/prettywriter.h>
#include <include/rapidjson/document.h>

namespace scratch {
    class Shader {
    public:
        // Read + compile shader
        Shader(const unsigned int Id, const std::string &name, const std::string &vertexPath, std::string fragmentPath);

        Shader() = default;


        const std::string &getVertexPath() const;

        const std::string &getFragmentPath() const;

        const std::string &getName() const;

        unsigned int getShaderId() const;

        unsigned int getId() const;

        // Activate shader
        void use() const;

        void reload();

        void setBool(const std::string &name, bool value) const;

        void setInt(const std::string &name, int value) const;

        void setUnsignedInt(const std::string &name, unsigned int value) const;

        void setFloat(const std::string &name, float value) const;

        void setMat4(const std::string &name, glm::mat4 value) const;

        void setVec3(const std::string &name, glm::vec3 value) const;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);


    private:
        unsigned int _id;
        unsigned int _shaderId;
        std::string _name;
        std::string _vertexPath;
        std::string _fragmentPath;

        std::string readFileContents(std::string filename);

        int compileShaders();

        void checkSuccessfulShaderCompilation(int shaderId);

        int generateAndCompileShader(std::string sourceFileLocation, int shaderType);

        void checkSuccessfulShaderLink(int shaderId);
    };
} // namespace scratch