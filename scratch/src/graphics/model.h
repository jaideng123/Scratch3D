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

namespace scratch {class Shader;}
namespace scratch {class Material;}


namespace scratch
{
    class Model
    {
    public:
        /*  Functions   */
        Model(std::string path);

        Model() = default;

        std::vector<scratch::Mesh> &getMeshes();

        const std::string &getModelPath() const;


    private:
        /*  Model Data  */
        std::vector<Mesh> meshes;
        std::vector<Material> materials;
        std::string directory;
        std::string modelPath;
        /*  Functions   */
        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);

        Material transformMaterial(aiMaterial *assimpMaterial);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);
    };
} // namespace scratch