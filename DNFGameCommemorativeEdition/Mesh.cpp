#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh()
{
    ClearMesh();
}

void
Mesh::CreateMesh(GLfloat* vertices,
                 unsigned int* indices,
                 unsigned int numOfVertices,
                 unsigned int numOfIndices)
{
    indexCount_ = numOfIndices;

    // VAO
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    // IBO (EBO)
    glGenBuffers(1, &IBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices[0]) * numOfIndices,
                 indices,
                 GL_STATIC_DRAW);

    // VBO
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    // Now IBO and the VBO are binded to VAO

    // x, y, z -> 3 value a vertex
    // location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // unbind VAO, VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Need to unbind IBO/EBO after VAO unbinded
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
Mesh::RenderMesh()
{
    glBindVertexArray(VAO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_);

    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
Mesh::ClearMesh()
{
    if (IBO_ != 0) {
        glDeleteBuffers(1, &IBO_);
        IBO_ = 0;
    }

    if (VBO_ != 0) {
        glDeleteBuffers(1, &VBO_);
        VBO_ = 0;
    }

    if (VAO_ != 0) {
        glDeleteVertexArrays(1, &VAO_);
        VAO_ = 0;
    }

    indexCount_ = 0;
}
