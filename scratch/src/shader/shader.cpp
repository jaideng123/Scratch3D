#include "shader.h"
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

scratch::Shader::Shader(const std::string vertexPath, const std::string fragmentPath)
{
    int vertexShader = generateAndCompileShader(vertexPath, GL_VERTEX_SHADER);
    _vertexPath = vertexPath;
    checkSuccessfulShaderCompilation(vertexShader);

    int fragmentShader = generateAndCompileShader(fragmentPath, GL_FRAGMENT_SHADER);
    _fragmentPath = fragmentPath;
    checkSuccessfulShaderCompilation(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkSuccessfulShaderLink(shaderProgram);

    ID = shaderProgram;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void scratch::Shader::use()
{
    glUseProgram(ID);
}

void scratch::Shader::reload()
{
    std::cout << "Reloading shader..." << std::endl;
    int vertexShader = generateAndCompileShader(_vertexPath, GL_VERTEX_SHADER);
    checkSuccessfulShaderCompilation(vertexShader);

    int fragmentShader = generateAndCompileShader(_fragmentPath, GL_FRAGMENT_SHADER);
    checkSuccessfulShaderCompilation(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkSuccessfulShaderLink(shaderProgram);

    std::cout << "Deleting Shader: " << ID << std::endl;
    glDeleteProgram(ID);
    std::cout << "Setting new Shader ID: " << shaderProgram << std::endl;
    ID = shaderProgram;
    std::cout << "Set new Shader ID: " << ID << std::endl;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::cout << "Finished reloading shader..." << std::endl;
    reloaded = true;
}

void scratch::Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void scratch::Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void scratch::Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void scratch::Shader::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void scratch::Shader::setVec3(const std::string &name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

int scratch::Shader::generateAndCompileShader(std::string sourceFileLocation, int shaderType)
{
    unsigned int shaderId;
    // Create Shader Object
    shaderId = glCreateShader(shaderType);

    // Read source code into a C std::string
    std::string shaderSource = readFileContents(sourceFileLocation);
    const char *vertexShaderSource = shaderSource.c_str();
    // Read source code into shader object
    glShaderSource(shaderId, 1, &vertexShaderSource, NULL);

    // Compile shader
    glCompileShader(shaderId);

    return shaderId;
}

std::string scratch::Shader::readFileContents(std::string filename)
{
    std::ifstream file;
    file.open(filename);
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    return fileStream.str();
}

void scratch::Shader::checkSuccessfulShaderCompilation(int shaderId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::scratch::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Shader Compilation Unsuccessful");
    }
}

void scratch::Shader::checkSuccessfulShaderLink(int shaderProgramId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::scratch::SHADER::PROGRAM::LINK_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Shader Linking Unsuccessful");
    }
}