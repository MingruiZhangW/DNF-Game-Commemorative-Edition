#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

class Floor : public GeometryNode
{
public:
    enum class FloorTextureType { Grass, Road };

    Floor(const std::string& name,
          ShaderProgram* shader,
          GLfloat width,
          GLfloat height,
          FloorTextureType textureType);
    ~Floor() {}

    void draw() override;
    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

private:
    ShaderProgram* m_shader;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    Texture m_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Floor Vertex Array Object
    GLuint m_floor_vao;
    // Floor Vertex Buffer Object
    GLuint m_floor_vbo;
};
