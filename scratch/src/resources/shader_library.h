#pragma once

#include <graphics/shader.h>
#include <memory>
#include <xxhash.h>
#include <unordered_map>
#include <vector>

namespace scratch {
    class ShaderLibrary {
    public:
        std::shared_ptr<Shader> addShader(const std::string& name, const std::string& vertexPath,const std::string& fragPath);
        std::shared_ptr<Shader> findShader(const std::string& name);

        const std::vector<std::string> getLoadedShaderNames() const;

        void reloadAllShaders();
    private:
        XXH64_hash_t const HASH_SEED = 0;
        std::unordered_map<XXH64_hash_t,std::shared_ptr<Shader>> loadedShaders;

    };
};