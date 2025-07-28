#include "triangle_mesh.h"

TriangleMesh::TriangleMesh() { setupMesh(); }

TriangleMesh::~TriangleMesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(2, VBOs.data());
  glDeleteBuffers(1, &EBO);
}

void TriangleMesh::setupMesh() {
  std::vector<float> positions = {
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f,
  };
  std::vector<int> colorIndicies = {0, 1, 2, 3};
  std::vector<int> elementIndicies = {0, 1, 2, 2, 1, 3};

  vertex_count = 6;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  VBOs.resize(2);

  glGenBuffers(2, VBOs.data());

  glGenBuffers(1, &EBO);

  // position
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // color indicies
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, colorIndicies.size() * sizeof(int),
               colorIndicies.data(), GL_STATIC_DRAW);
  glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void *)0);
  glEnableVertexAttribArray(1);

  // element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementIndicies.size() * sizeof(int),
               elementIndicies.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void TriangleMesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}