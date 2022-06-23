#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:
    Shader();
    ~Shader();

    void CreateFromString(const char* vertexCode, const char* fragmentCode);
    void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

    std::string ReadFile(const char* fileLocation);

    GLuint GetProjectionLocation();
    GLuint GetModelLocation();

    void UseShader();
    void ClearShader();

    static void UnUseShader()
    {
        // Unassign the shader
        glUseProgram(0);
    }

private:
    // uniformModel, uniformProjection are for model mat and projection mat
    GLuint shaderID_ {0};
    GLuint uniformProjection_ {0};
    GLuint uniformModel_ {0};

    void CompileShader(const char* vertexCode, const char* fragmentCode);
    void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};
