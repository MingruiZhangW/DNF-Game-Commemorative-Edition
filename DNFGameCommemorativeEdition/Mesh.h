#pragma once

#include <GL/glew.h>

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void CreateMesh(GLfloat* vertices,
                    unsigned int* indices,
                    unsigned int numOfVertices,
                    unsigned int numOfIndices);
    void RenderMesh();
    void ClearMesh();

private:
    GLuint VAO_ {0};
    GLuint VBO_ {0};
    GLuint IBO_ {0};
    GLsizei indexCount_ {0};
};
