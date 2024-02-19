#include "TerrainChunk.h"
#include "PerlinNoise.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>


TerrainChunk::TerrainChunk(glm::vec3 pos, int w, int h) : position(pos), width(w), height(h) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

TerrainChunk::~TerrainChunk() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

TerrainChunk::TerrainChunk(TerrainChunk&& other) noexcept
    : position(std::move(other.position)), width(other.width), height(other.height),
    VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), vertices(std::move(other.vertices)), indices(std::move(other.indices)) {
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

TerrainChunk& TerrainChunk::operator=(TerrainChunk&& other) noexcept {
    if (this != &other) {
        // Free existing resources
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        // Transfer ownership and invalidate the source object
        position = std::move(other.position);
        width = other.width;
        height = other.height;
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);

        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

void TerrainChunk::generateTerrain(float* noise) {
    generateVertices(noise);
    generateIndices();
    generateBuffers();
}

glm::vec3 TerrainChunk::getPosition() const {
    return position;
}

void TerrainChunk::draw(Shader& shader) {
    GLenum err;

    int success;
    glGetShaderiv(shader.ID, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shader.ID, GL_COMPILE_STATUS, &success);
    GLenum errorBefore = glGetError();
    glBindVertexArray(VAO);
    GLenum errorAfter= glGetError();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void TerrainChunk::generateVertices(float* noise) {
    const int verticesPerRow = width + 1;
    const int verticesPerCol = height + 1;
    vertices.clear();
    vertices.reserve(verticesPerRow * verticesPerCol * 3);

    for (int i = 0; i < verticesPerCol; i++) {
        for (int j = 0; j < verticesPerRow; j++) {
            vertices.push_back((float)j / width + position.x);
            vertices.push_back((float)i / height + position.y);
            if (i < height && j < width) {
                vertices.push_back(noise[i * width + j]); // z-coordinate from noise
            }
            else {
                vertices.push_back(0.0f);
            }
        }
    }
}

void TerrainChunk::generateIndices() {
    indices.clear();
    indices.reserve(width * height * 6);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int base = y * (width + 1) + x;
            indices.push_back(base);
            indices.push_back(base + width + 1);
            indices.push_back(base + 1);
            indices.push_back(base + 1);
            indices.push_back(base + width + 1);
            indices.push_back(base + width + 2);
        }
    }
}

void TerrainChunk::generateBuffers() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}