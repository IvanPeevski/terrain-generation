#pragma once
#ifndef TERRAIN_CHUNK_H
#define TERRAIN_CHUNK_H

#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

class TerrainChunk {
public:
    TerrainChunk(glm::vec3 pos, int w, int h);
    ~TerrainChunk();
    TerrainChunk(TerrainChunk&& other) noexcept;
    TerrainChunk& operator=(TerrainChunk&& other) noexcept;
    void generateTerrain(float* noise);
    void draw(Shader& shader);
    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    int width, height;
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    void generateVertices(float* noise);
    void generateIndices();
    void generateBuffers();
};

#endif

