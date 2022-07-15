#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

class Dialog : public GeometryNode
{
public:
    Dialog(ShaderProgram* shader, GLfloat windowWidth, GLfloat windowHeight);
    ~Dialog() {}

    void draw() override;

    glm::vec2 getOriginTrans();
    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

private:
    ShaderProgram* m_shader;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    GLfloat m_window_width;
    GLfloat m_window_height;

    Texture m_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Dialog Vertex Array Object
    GLuint m_dialog_vao;
    // Dialog Vertex Buffer Object
    GLuint m_dialog_vbo;
};
