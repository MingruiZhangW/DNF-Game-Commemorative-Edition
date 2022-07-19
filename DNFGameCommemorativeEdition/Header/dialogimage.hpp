#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

class DialogImage : public GeometryNode
{
public:
    DialogImage(ShaderProgram* shader);
    ~DialogImage() {}

    void draw() override;

    void setDialogOrigin(glm::vec2 dialogOrigin);

    void setShouldDraw(bool show)
    {
        m_should_draw = show;
    }

    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

    void cleanMovement();

private:
    ShaderProgram* m_shader;

    bool m_should_draw;

    glm::vec2 m_dialog_origin;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    Texture m_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Dialog Image Vertex Array Object
    GLuint m_dialog_img_vao;
    // Dialog Image Vertex Buffer Object
    GLuint m_dialog_img_vbo;
};
