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
#include "graphics/material.h"
#include <include/rapidjson/document.h>


namespace scratch {
    class Model {
    public:
        Model(unsigned int id, const std::string &path);

        Model();

        unsigned int getId() const;

        std::vector<scratch::Mesh> &getMeshes();

        const std::string &getModelPath() const;

        void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

        void deserialize(const rapidjson::Value &object);

        void setDefaultShader(const std::shared_ptr<scratch::Shader> &defaultShader);

        const std::vector<std::shared_ptr<scratch::Material>> &scratch::Model::getDefaultMaterials() const;

    private:
        unsigned int _id;

        /*  Model Data  */
        std::vector<Mesh> _meshes;
        std::vector<std::shared_ptr<Material>> _defaultMaterials;
        std::string _directory;
        std::string _modelPath;

        /*  Functions   */
        void loadModel(const std::string &path);

        void processNode(aiNode *node, const aiScene *scene);

        std::shared_ptr<scratch::Material> scratch::Model::transformMaterial(aiMaterial *assimpMaterial);

        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        void scratch::Model::attachMaterialTextures(const std::shared_ptr<scratch::Material> material,
                                                    const aiMaterial *assimpMaterial,
                                                    const aiTextureType &type,
                                                    const std::string &typeName);
    };
} // namespace scratch