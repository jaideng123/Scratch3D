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
        Material material = transformMaterial(scene->mMaterials[i]);
        _materials.push_back(material);
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

scratch::Material scratch::Model::transformMaterial(aiMaterial *assimpMaterial) {
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    // 1. diffuse maps
    std::vector<Texture> textures = std::vector<Texture>();

    std::vector<Texture> diffuseMaps = loadMaterialTextures(assimpMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(assimpMaterial, aiTextureType_SPECULAR,
                                                             "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(assimpMaterial, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(assimpMaterial, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Material(textures);
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
    return Mesh(vertices, indices, &_materials[mesh->mMaterialIndex]);
}

std::vector<scratch::Texture> scratch::Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                                   const std::string &typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = textureFromFile(str.C_Str(), _directory);
        texture.type = typeName;
        texture.path = _directory + "/" + str.C_Str();
        textures.push_back(texture);
    }
    return textures;
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

    writer.String("materials");
    writer.StartArray();
    for (auto &material : _materials) {
        material.serialize(writer);
    }
    writer.EndArray();

    writer.String("defaultShaderId");
    writer.Uint(_defaultShader->getId());

    writer.EndObject();
}

void scratch::Model::deserialize(const rapidjson::Value &object) {
    _id = object["id"].GetUint();
    _modelPath = object["modelPath"].GetString();
    this->loadModel(_modelPath);
    //TODO handle materials
}

scratch::Model::Model() {
}

const std::shared_ptr<scratch::Shader> &scratch::Model::getDefaultShader() const {
    return _defaultShader;
}

void scratch::Model::setDefaultShader(const std::shared_ptr<scratch::Shader> &defaultShader) {
    Model::_defaultShader = defaultShader;
    for (auto &mesh : _meshes) {
        mesh.getMaterial()->setShader(defaultShader);
    }
}

unsigned int scratch::Model::getId() const {
    return _id;
}

glm::vec3 convertVector3(aiVector3D aiVec3) {
    auto newVec3 = glm::vec3(0);
    newVec3.x = aiVec3.x;
    newVec3.y = aiVec3.y;
    newVec3.z = aiVec3.z;
    return newVec3;
}

unsigned int textureFromFile(const std::string &path, const std::string &directory, bool gamma) {
    return textureFromFile(path, directory, gamma, GL_REPEAT);
}

unsigned int textureFromFile(const std::string &path, const std::string &directory, bool gamma, int wrapMode) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

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