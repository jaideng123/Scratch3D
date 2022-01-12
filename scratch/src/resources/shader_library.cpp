//
// Created by JJJai on 12/18/2021.
//

#include <iostream>
#include <utilities/assert.h>
#include "shader_library.h"

std::shared_ptr<scratch::Shader>
scratch::ShaderLibrary::addShader(const std::string &name, const std::string &vertexPath, const std::string &fragPath) {
    std::cout << "Loading Shader: " << name << std::endl;
    XXH64_hash_t hash = XXH64(name.c_str(), name.length(), HASH_SEED);
    std::cout << "Generated Hash:" << std::to_string(hash) << std::endl;
    std::shared_ptr<scratch::Shader> shader = std::make_shared<scratch::Shader>(hash, name, vertexPath,
                                                                                fragPath);
    if (loadedShaders.find(hash) != loadedShaders.end()) {
        std::cout << "Warning: Shader already loaded, replacing..." << std::to_string(hash) << std::endl;
    }
    loadedShaders.insert({hash, shader});
    return shader;
}

std::shared_ptr<scratch::Shader> scratch::ShaderLibrary::findShader(const std::string &name) {
    std::cout << "Looking Up Shader: " << name << std::endl;
    XXH64_hash_t hash = XXH64(name.c_str(), name.length(), HASH_SEED);
    std::cout << "Generated Hash:" << std::to_string(hash) << std::endl;
    if (loadedShaders.find(hash) != loadedShaders.end()) {
        return loadedShaders[hash];
    } else {
        SCRATCH_ASSERT_NEVER("Could not find shader: " + name);
    }
}

void scratch::ShaderLibrary::reloadAllShaders() {
    for (auto pair: loadedShaders) {
        pair.second->reload();
    }
}

const std::vector<std::string> scratch::ShaderLibrary::getLoadedShaderNames() const {
    std::vector<std::string> shaders;
    shaders.reserve(loadedShaders.size());
    for(const auto& kv : loadedShaders){
        shaders.push_back(kv.second->getName());
    }
    return shaders;
}
