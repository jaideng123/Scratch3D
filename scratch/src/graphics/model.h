#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "graphics/mesh.hpp"
#include "graphics/material.hpp"
#include <include/rapidjson/document.h>


namespace scratch { class Shader; }
namespace scratch { class Material; }


namespace scratch {
    class Model {
    public:
        unsigned int Id;

        /*  Functions   */
        Model(unsigned int id, std::string path);

        Model();

        std::vector<scratch::Mesh> &getMeshes();

        const std::string &getModelPath() const;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);

        const std::shared_ptr<scratch::Shader> &getDefaultShader() const;

        void setDefaultShader(const std::shared_ptr<scratch::Shader> &defaultShader);

    private:
        /*  Model Data  */
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
        std::string directory;
        std::string modelPath;
        std::shared_ptr<scratch::Shader> defaultShader;

        /*  Functions   */
        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);

        Material transformMaterial(aiMaterial *assimpMaterial);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);
    };
} // namespace scratch