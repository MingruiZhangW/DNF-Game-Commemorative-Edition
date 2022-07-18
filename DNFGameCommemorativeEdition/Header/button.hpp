#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

#include <glm.hpp>

class Button : public GeometryNode
{
public:
    enum class ButtonTextureType {
        ExitButton,
        PlayButton,
        BackButton,
        ControlButton,
        Logo,
        Victory,
        ZKey,
        XKey,
        ArrowKey,
        MouseLeft,
        AttackLabel,
        MoveLabel,
        InteractLabel
    };

    Button(const std::string& name, ShaderProgram* shader, ButtonTextureType textureType);
    ~Button() {}

    void draw() override;
    void translate(const glm::vec3& amount) override;

    // If mouse is on top of the Button.
    bool checkOnTop();
    bool checkOnTop(const glm::vec2& mousePos);

    float getButtonDx()
    {
        return m_button_dx;
    }

    float getButtonDy()
    {
        return m_button_dy;
    }

    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture_normal.getTextureWidth(), m_texture_normal.getTextureHeight());
    }

    // For center buttons in certain scene
    void translateToWindowCenter(float dx, float windowWidth, float windowHeight);

    void cleanMovement();

private:
    ShaderProgram* m_shader;

    Texture m_texture_normal;
    Texture m_texture_hover;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Help to process mouse hover, click
    bool m_button_is_hovered;
    GLfloat m_button_dx;
    GLfloat m_button_dy;

    ButtonTextureType m_current_button_t_type;

    // Button Vertex Array Object
    GLuint m_button_vao;
    // Button Vertex Buffer Object
    GLuint m_button_vbo;
};
