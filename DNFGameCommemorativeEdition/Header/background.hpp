#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

#include <glm.hpp>

class Background : public GeometryNode
{
public:
    Background(const std::string& name);
    ~Background() {}

    void init(ShaderProgram* shader, GLfloat width, GLfloat height);
    void draw();

private:
    ShaderProgram* m_shader;

    Texture m_back_texture;
    Texture m_mid_texture;

    GLfloat m_background_width;
    GLfloat m_background_height;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Background Vertex Array Object
    GLuint m_background_vao;
    // Background Vertex Buffer Object
    GLuint m_background_vbo;
};
