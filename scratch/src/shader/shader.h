#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace scratch
{
    class Shader
    {
    public:
        unsigned int ID;
        bool reloaded;

        // Read + compile shader
        Shader(const std::string vertexPath, const std::string fragmentPath);
        Shader() = default;
        // Activate shader
        void use();

        void reload();

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setMat4(const std::string &name, glm::mat4 value) const;
        void setVec3(const std::string &name, glm::vec3 value) const;

    private:
        std::string _vertexPath;
        std::string _fragmentPath;
        std::string readFileContents(std::string filename);
        void checkSuccessfulShaderCompilation(int shaderId);
        int generateAndCompileShader(std::string sourceFileLocation, int shaderType);
        void checkSuccessfulShaderLink(int shaderId);
    };
} // namespace scratch