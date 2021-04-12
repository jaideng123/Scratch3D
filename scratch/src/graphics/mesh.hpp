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
#include "graphics/material.hpp"

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
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> material) {
            this->_vertices = std::move(vertices);
            this->_indices = std::move(indices);
            this->_material = material;

            // now that we have all the required data, set the vertex buffers and its attribute pointers.
            setupMesh();
        }

        // render the mesh
        void draw() {
            // draw mesh
            glBindVertexArray(_vao);
            glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        std::shared_ptr<Material> getMaterial() const {
            return _material;
        }

    private:
        /*  Mesh Data  */
        std::vector<Vertex> _vertices;
        std::vector<unsigned int> _indices;
        std::shared_ptr<Material> _material;
        unsigned int _vao;

        /*  Render data  */
        unsigned int _vbo, _ebo;

        /*  Functions    */
        // initializes all the buffer objects/arrays
        void setupMesh() {
            // create buffers/arrays
            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &_vbo);
            glGenBuffers(1, &_ebo);

            glBindVertexArray(_vao);
            // load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            // A great thing about structs is that their memory layout is sequential for all its items.
            // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            // again translates to 3/2 floats which translates to a byte array.
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), &_indices[0], GL_STATIC_DRAW);

            // set the vertex attribute pointers
            // vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tangent));
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, bitangent));

            glBindVertexArray(0);
        }
    };
} // namespace scratch
