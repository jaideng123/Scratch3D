#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <vector>

#include "shader.h"
#include "graphics/material.h"

namespace scratch {

    struct Vertex {
        // position
        glm::vec3 position;
        // texCoords
        glm::vec2 texCoords;
        // normal
        glm::vec3 normal;
        // tangent
        glm::vec3 tangent;
        // bitangent
        glm::vec3 bitangent;
    };

    class Mesh {
    public:

        /*  Functions  */
        // constructor
        Mesh(std::vector<Vertex> vertices,
             std::vector<unsigned int> indices,
             std::shared_ptr<Material> material,
             const unsigned int materialIndex);

        // render the mesh
        void draw();

        void setMaterial(const std::shared_ptr<Material> &material);

        std::shared_ptr<Material> getMaterial() const;

        unsigned int getMaterialIndex() const;

    private:
        /*  Mesh Data  */
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::shared_ptr<Material> _material;
        unsigned int _materialIndex;
        unsigned int _vao;

        /*  Render data  */
        unsigned int _vbo, _ebo;

        /*  Functions    */
        // initializes all the buffer objects/arrays
        void setupMesh();
    };
} // namespace scratch
