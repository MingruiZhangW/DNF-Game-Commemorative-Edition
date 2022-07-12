#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"
#include "constant.hpp"

class DialogChar : public GeometryNode
{
public:
    enum class CharType { English, Chinese };

    DialogChar(const std::string& name,
               ShaderProgram* shader,
               const Fonts::CharacterInfo& charInfo,
               GLfloat scale,
               CharType charType = CharType::English);
    ~DialogChar() {}

    void draw();

private:
    ShaderProgram* m_shader;

    GLfloat m_char_scale_x;
    GLfloat m_char_scale_y;

    Fonts::CharacterInfo m_char_info;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // DialogChar Vertex Array Object
    GLuint m_dialog_char_vao;
    // DialogChar Vertex Buffer Object
    GLuint m_dialog_char_vbo;

    // Uniform drawText id
    GLuint m_draw_text_id;

    CharType m_char_type;
};
