#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "scenemanager.hpp"

class Floor : public SceneNode
{
public:
    Floor(const std::string& name);
    ~Floor() {}

    void init(ShaderProgram* shader, GLfloat width, GLfloat height);
    void draw();

private:
    ShaderProgram* m_shader;

    GLfloat m_window_width;
    GLfloat m_window_height;

    Texture m_grass_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Floor Vertex Array Object
    GLuint m_floor_vao;
    // Floor Vertex Buffer Object
    GLuint m_floor_vbo;
};
