//
// Created by JJJai on 12/17/2021.
//

#include "resource_manager.h"

std::shared_ptr<scratch::Model> scratch::ResourceManager::loadModel(const std::string &path) {
    XXH64_hash_t hash = XXH64(path.c_str(), path.length(), HASH_SEED);
    std::cout << "Generated Hash:" << std::to_string(hash) << std::endl;
    if(loadedModels.find(hash) != loadedModels.end()){
        std::cout << "Cache Hit!" << std::to_string(hash) << std::endl;
        return loadedModels[hash];
    }
    std::shared_ptr<scratch::Model> newModel = std::make_shared<scratch::Model>(hash, path);
    loadedModels.insert({hash,newModel});
    return newModel;
}
