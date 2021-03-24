#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <include/rapidjson/document.h>


scratch::Shader::Shader(const unsigned int Id, const std::string vertexPath, const std::string fragmentPath) {
    this->Id = Id;

    _vertexPath = vertexPath;
    _fragmentPath = fragmentPath;

    shaderId = compileShaders();
}

void scratch::Shader::use() {
    glUseProgram(shaderId);
}

void scratch::Shader::reload() {
    std::cout << "Reloading shader..." << std::endl;
    int newShaderId = compileShaders();

    std::cout << "Deleting Shader: " << shaderId << std::endl;
    glDeleteProgram(shaderId);
    std::cout << "Setting new Shader ID: " << newShaderId << std::endl;
    shaderId = newShaderId;
    std::cout << "Set new Shader ID: " << shaderId << std::endl;

    std::cout << "Finished reloading shader..." << std::endl;
}

int scratch::Shader::compileShaders() {
    std::cout << "Compiling shaders..." << std::endl;
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void scratch::Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (int) value);
}

void scratch::Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
}

void scratch::Shader::setUnsignedInt(const std::string &name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(shaderId, name.c_str()), value);
}

void scratch::Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
}

void scratch::Shader::setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void scratch::Shader::setVec3(const std::string &name, glm::vec3 value) const {
    glUniform3f(glGetUniformLocation(shaderId, name.c_str()), value.x, value.y, value.z);
}

int scratch::Shader::generateAndCompileShader(std::string sourceFileLocation, int shaderType) {
    unsigned int shaderId;
    // Create Shader Object
    shaderId = glCreateShader(shaderType);

    // Read source code into a C std::string
    std::string shaderSource = readFileContents(sourceFileLocation);
    const char *vertexShaderSource = shaderSource.c_str();
    // Read source code into shader object
    glShaderSource(shaderId, 1, &vertexShaderSource, nullptr);

    // Compile shader
    glCompileShader(shaderId);

    return shaderId;
}

std::string scratch::Shader::readFileContents(std::string filename) {
    std::cout << std::filesystem::current_path() << std::endl;
    std::ifstream file;
    file.open(filename);
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    file.close();

    return fileStream.str();
}

void scratch::Shader::checkSuccessfulShaderCompilation(int shaderId) {
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::scratch::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Shader Compilation Unsuccessful");
    }
}

void scratch::Shader::checkSuccessfulShaderLink(int shaderProgramId) {
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::scratch::SHADER::PROGRAM::LINK_FAILED\n"
                  << infoLog << std::endl;
        throw std::runtime_error("Shader Linking Unsuccessful");
    }
}

void scratch::Shader::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("id");
    writer.Uint(Id);

    writer.String("vertexPath");
    writer.String(_vertexPath.c_str(), static_cast<rapidjson::SizeType>(_vertexPath.length()));

    writer.String("fragmentPath");
    writer.String(_fragmentPath.c_str(), static_cast<rapidjson::SizeType>(_fragmentPath.length()));

    writer.EndObject();
}

void scratch::Shader::deserialize(const rapidjson::Value &object) {
    Id = object["id"].GetUint();
    _vertexPath = object["vertexPath"].GetString();
    _fragmentPath = object["fragmentPath"].GetString();
    shaderId = compileShaders();
}

const std::string &scratch::Shader::getVertexPath() const {
    return _vertexPath;
}

const std::string &scratch::Shader::getFragmentPath() const {
    return _fragmentPath;
}


