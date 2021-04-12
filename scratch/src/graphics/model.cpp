#include <string>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

unsigned int textureFromFile(const std::string &path, const std::string &directory, bool gamma = false);

unsigned int textureFromFile(const std::string &path, const std::string &directory, bool gamma, int wrapMode);

glm::vec3 convertVector3(aiVector3D aiVec3);

scratch::Model::Model(unsigned int id, const std::string &path) {
    _id = id;
    _modelPath = path;
    loadModel(path);
}

std::vector<scratch::Mesh> &scratch::Model::getMeshes() {
    return _meshes;
}

void scratch::Model::loadModel(const std::string &path) {
    Assimp::Importer import;
    // Import scene data (Triangulate = Make all faces 3 indices(x,y,z))
    const aiScene *scene = import.ReadFile(path,
                                           aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    // We assume that all textures are in the same directory as the scene
    _directory = path.substr(0, path.find_last_of('/'));

    for (size_t i = 0; i < scene->mNumMaterials; ++i) {
        _materials.push_back(transformMaterial(scene->mMaterials[i]));
    }

    processNode(scene->mRootNode, scene);
}

void scratch::Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh convertedMesh = processMesh(mesh, scene);
        _meshes.push_back(convertedMesh);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<scratch::Material> scratch::Model::transformMaterial(aiMaterial *assimpMaterial) {
    auto material = std::make_shared<Material>();
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    // 1. diffuse maps
    attachMaterialTextures(material, assimpMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
    // 2. specular maps
    attachMaterialTextures(material, assimpMaterial, aiTextureType_SPECULAR, "texture_specular");
    // 3. normal maps
    attachMaterialTextures(material, assimpMaterial, aiTextureType_HEIGHT, "texture_normal");
    // 4. height maps
    attachMaterialTextures(material, assimpMaterial, aiTextureType_AMBIENT, "texture_height");

    return material;
}

void scratch::Model::attachMaterialTextures(const std::shared_ptr<scratch::Material> material,
                                            const aiMaterial *assimpMaterial,
                                            const aiTextureType &type,
                                            const std::string &typeName) {
    for (unsigned int i = 0; i < assimpMaterial->GetTextureCount(type); i++) {
        aiString str;
        assimpMaterial->GetTexture(type, i, &str);
        std::string texturePath = _directory + "/" + str.C_Str();
        material->addTexture(texturePath, typeName);
    }
}

scratch::Mesh scratch::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        // positions
        vertex.position = convertVector3(mesh->mVertices[i]);
        // normals
        vertex.normal = convertVector3(mesh->mNormals[i]);
        // texture coordinates
        if (mesh->HasTextureCoords(0)) {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        } else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        if (mesh->HasTangentsAndBitangents()) {
            // tangent
            vertex.tangent = convertVector3(mesh->mTangents[i]);
            // bitangent
            vertex.bitangent = convertVector3(mesh->mBitangents[i]);
        }
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, _materials[mesh->mMaterialIndex], mesh->mMaterialIndex);
}

const std::string &scratch::Model::getModelPath() const {
    return _modelPath;
}

void scratch::Model::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) {
    writer.StartObject();

    writer.String("id");
    writer.Uint(_id);

    writer.String("modelPath");
    writer.String(_modelPath.c_str(), static_cast<rapidjson::SizeType>(_modelPath.length()));

    writer.String("materialIds");
    writer.StartArray();
    for (auto material : _materials) {
        writer.Uint(material->getId());
    }
    writer.EndArray();

    writer.EndObject();
}

void scratch::Model::deserialize(const rapidjson::Value &object) {
    _id = object["id"].GetUint();
    _modelPath = object["modelPath"].GetString();
    this->loadModel(_modelPath);
}

scratch::Model::Model() {
}

void scratch::Model::setDefaultShader(const std::shared_ptr<scratch::Shader> &defaultShader) {
    for (auto &material : _materials) {
        material->setShader(defaultShader);
    }
}

unsigned int scratch::Model::getId() const {
    return _id;
}

const std::vector<std::shared_ptr<scratch::Material>> &scratch::Model::getMaterials() const {
    return _materials;
}

void scratch::Model::swapMaterial(const unsigned int index, const std::shared_ptr<scratch::Material> newMaterial) {
    for (auto &mesh : _meshes) {
        if (mesh.getMaterialIndex() == index) {
            mesh.setMaterial(newMaterial);
        }
    }
    _materials[index] = newMaterial;
}

glm::vec3 convertVector3(aiVector3D aiVec3) {
    auto newVec3 = glm::vec3(0);
    newVec3.x = aiVec3.x;
    newVec3.y = aiVec3.y;
    newVec3.z = aiVec3.z;
    return newVec3;
}