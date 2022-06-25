#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"

class Floor
{
public:
    Floor();
    ~Floor() {}

    void init(ShaderProgram* shader);
    void draw();

private:
    ShaderProgram* m_shader;

    Texture m_grass_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    GLuint m_floor_vao; // Floor Vertex Array Object
    GLuint m_floor_vbo; // Floor Vertex Buffer Object
};
