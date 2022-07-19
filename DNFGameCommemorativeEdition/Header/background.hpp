#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

#include <glm.hpp>

/*
 * Layer zero
 */
class Background : public GeometryNode
{
public:
    /*
     * For Scene Zero, Only a simple background is needed,
     * for scene one and scene two, far and mid are both needed.
     */
    enum class BackgroundTextureType { Mid, Far, SceneZero };

    Background(const std::string& name,
               ShaderProgram* shader,
               GLfloat width,
               GLfloat height,
               BackgroundTextureType textureType);
    ~Background() {}

    void draw() override;

    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

private:
    ShaderProgram* m_shader;

    Texture m_texture;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    // Attribute
    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Background Vertex Array Object
    GLuint m_background_vao;
    // Background Vertex Buffer Object
    GLuint m_background_vbo;
};
