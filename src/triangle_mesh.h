#pragma once
#include "config.h"

class TriangleMesh {
public:
    // создаёт и настраивает (VAO+VBO).
    TriangleMesh();
    // удаляет буферы
    ~TriangleMesh();
    // команда отрисовки
    void draw();

private:
    unsigned int EBO, VAO, vertex_count;
    void setupMesh();
    std::vector<unsigned int> VBOs;
};