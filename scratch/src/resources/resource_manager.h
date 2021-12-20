#pragma once

#include <memory>
#include <graphics/model.h>
#include <unordered_map>
#include <xxhash.h>

namespace scratch {
    class ResourceManager {
    public:
        std::shared_ptr<Model> loadModel(const std::string& path);
        std::shared_ptr<Texture> loadTexture(const std::string &path,const std::string &type);
//        std::shared_ptr<Material> loadMaterial(const std::string& path);
    private:
        XXH64_hash_t const HASH_SEED = 0;
        std::unordered_map<XXH64_hash_t,std::shared_ptr<Model>> loadedModels;
        std::unordered_map<XXH64_hash_t,std::shared_ptr<Texture>> loadedTextures;
//        std::unordered_map<XXH64_hash_t,std::shared_ptr<Material>> loadedMaterials;
        unsigned int textureFromFile(const std::string &path, bool gamma = false);

        unsigned int textureFromFile(const std::string &path, bool gamma, int wrapMode);

    };
}