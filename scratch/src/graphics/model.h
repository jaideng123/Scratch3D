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


namespace scratch {
    class Model {
    public:
        Model(unsigned int id, const std::string& path);

        Model();

        unsigned int getId() const;

        std::vector<scratch::Mesh> &getMeshes();

        const std::string &getModelPath() const;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);

        const std::shared_ptr<scratch::Shader> &getDefaultShader() const;

        void setDefaultShader(const std::shared_ptr<scratch::Shader> &defaultShader);

    private:
        unsigned int _id;

        /*  Model Data  */
        std::vector<Mesh> _meshes;
        std::vector<Material> _materials;
        std::string _directory;
        std::string _modelPath;
        std::shared_ptr<scratch::Shader> _defaultShader;

        /*  Functions   */
        void loadModel(const std::string& path);

        void processNode(aiNode *node, const aiScene *scene);

        Material transformMaterial(aiMaterial *assimpMaterial);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  const std::string &typeName);
    };
} // namespace scratch