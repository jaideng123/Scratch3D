//
// Created by JJJai on 12/17/2021.
//

#include "resource_manager.h"

std::shared_ptr<scratch::Model> scratch::ResourceManager::loadModel(const std::string &path) {
    std::cout << "Loading Model: " << path << std::endl;
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

std::shared_ptr<scratch::Texture> scratch::ResourceManager::loadTexture(const std::string &path,const std::string &type) {
    std::cout << "Loading Texture: " << path + "_" + type << std::endl;
    std::string identifier = path + "_" + type;
    XXH64_hash_t hash = XXH64(identifier.c_str(), identifier.length(), HASH_SEED);
    std::cout << "Generated Hash:" << std::to_string(hash) << std::endl;
    if (loadedTextures.find(hash) != loadedTextures.end()) {
        std::cout << "Cache Hit!" << std::to_string(hash) << std::endl;
        return loadedTextures[hash];
    }
    std::shared_ptr<scratch::Texture> newTexture = std::make_shared<scratch::Texture>();
    newTexture->id = textureFromFile(path);
    newTexture->type = type;
    newTexture->path = path;
    return newTexture;
}

unsigned int scratch::ResourceManager::textureFromFile(const std::string &path, bool gamma, int wrapMode) {
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

unsigned int scratch::ResourceManager::textureFromFile(const std::string &path, bool gamma) {
    return textureFromFile(path, gamma, GL_REPEAT);
}

