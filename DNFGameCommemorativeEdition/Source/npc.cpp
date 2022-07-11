#include "npc.hpp"
#include "gamewindow.hpp"
#include "glerrorcheck.hpp"

#include <fstream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// clang-format off

// NPC vertex at the live area origin
static GLfloat npc_vertex_buffer_data[] =
{
    // triangle 1
    // x,    y,    z
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    // triangle 2
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
};

// NPC texture_coord
static GLfloat npc_texture_coord_data[] =
{
    // triangle 1
    // u,    v
    1.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    // triangle 2
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f
};
// Shadow shift const
static const float m_npc_shadow_shift_x{55.0f};
static const float m_npc_shadow_shift_y{-170.0f};

// Collide const
static const float m_npc_collide_height{15.0f};
static const float m_npc_collide_y_offset{30.0f};

// Hover offset const
static const float m_npc_hover_x_offset{30.0f};
static const float m_npc_hover_y_offset{30.0f};

// clang-format on

NPC::NPC(ShaderProgram* shader)
    : GeometryNode(StringContant::npcName)
    , m_shader(shader)
    , m_npc_center(SpriteSize::npcWidth / 2.0f, SpriteSize::npcWidth / 2.0f)
    , m_npc_show_outline(false)
    , m_animation_speed(0.1f)
    , m_current_frame("0")
    , m_animation_cursor(0.0f)
    , m_dialog_box_triggered(false)
{
    // Create shadow shader
    m_shadow_shader.generateProgramObject();
    m_shadow_shader.attachVertexShader(ShadowShaderPath::vertexShader.c_str());
    m_shadow_shader.attachFragmentShader(ShadowShaderPath::fragmentShader.c_str());
    m_shadow_shader.link();

    // Set up the uniforms
    m_shadow_p_uni = m_shadow_shader.getUniformLocation("P");
    m_shadow_v_uni = m_shadow_shader.getUniformLocation("V");
    m_shadow_m_uni = m_shadow_shader.getUniformLocation("M");

    // Load texture - stand
    std::ifstream ifs_s(TexturePath::npcJsonPath);
    m_npc_animation_json_parser = json::parse(ifs_s);
    m_number_of_frames = static_cast<unsigned int>(
        m_npc_animation_json_parser[SSJsonKeys::frames].size());
    m_textures_sheet = Texture(TexturePath::npcPNGPath);
    m_textures_sheet.loadTexture();

    // Load texture - walk
    m_npc_width = SpriteSize::npcWidth;
    m_npc_height = SpriteSize::npcHeight;

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(m_npc_width, m_npc_height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_npc_vao);
    glBindVertexArray(m_npc_vao);

    // Create the floor vertex buffer
    glGenBuffers(1, &m_npc_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_npc_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(npc_vertex_buffer_data),
                 npc_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Texture vbo
    glGenBuffers(1, &m_npc_tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_npc_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(npc_texture_coord_data),
                 npc_texture_coord_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_npc_vbo);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(npc_vertex_buffer_data[0]) * 3,
                          nullptr);

    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_npc_tex_vbo);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(npc_texture_coord_data[0]) * 2,
                          nullptr);

    m_show_outline_id = m_shader->getUniformLocation("showOutline");

    updateTexCoord();

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
NPC::updateFrame()
{
    m_animation_cursor = m_animation_cursor + GameWindow::getDeltaTime();

    if (m_animation_cursor > m_animation_speed) {
        m_current_frame = std::to_string((std::stoi(m_current_frame) + 1) % m_number_of_frames);
        m_animation_cursor = 0;
    } else
        return;

    updateTexCoord();
}

void
NPC::updateTexCoord()
{
    float texX, texY, texW, texH;

    // get sprite sheet coord
    texX = m_npc_animation_json_parser[SSJsonKeys::frames][m_current_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::x]
                                          .get<float>();
    texY = m_npc_animation_json_parser[SSJsonKeys::frames][m_current_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::y]
                                          .get<float>();
    texW = m_npc_animation_json_parser[SSJsonKeys::frames][m_current_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::w]
                                          .get<float>();
    texH = m_npc_animation_json_parser[SSJsonKeys::frames][m_current_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::h]
                                          .get<float>();

    // update each data point to tex coord
    npc_texture_coord_data[0] = (texX + texW) / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[1] = (texY + texH) / m_textures_sheet.getTextureHeight();

    npc_texture_coord_data[2] = texX / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[3] = (texY + texH) / m_textures_sheet.getTextureHeight();

    npc_texture_coord_data[4] = (texX + texW) / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[5] = texY / m_textures_sheet.getTextureHeight();

    npc_texture_coord_data[6] = texX / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[7] = texY / m_textures_sheet.getTextureHeight();

    npc_texture_coord_data[8] = texX / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[9] = (texY + texH) / m_textures_sheet.getTextureHeight();

    npc_texture_coord_data[10] = (texX + texW) / m_textures_sheet.getTextureWidth();
    npc_texture_coord_data[11] = texY / m_textures_sheet.getTextureHeight();

    glBindVertexArray(m_npc_vao);

    // texture coord
    glBindBuffer(GL_ARRAY_BUFFER, m_npc_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(npc_texture_coord_data),
                 npc_texture_coord_data,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
NPC::draw()
{
    updateFrame();

    /* Draw shadow texture */
    m_shader->disable();
    m_shadow_shader.enable();

    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textures_sheet.useTexture();

    glBindVertexArray(m_npc_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    m_shadow_shader.disable();
    m_shader->enable();

    /* Draw actual texture */

    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textures_sheet.useTexture();

    glBindVertexArray(m_npc_vao);
    // Outline here
    glUniform1i(m_show_outline_id, m_npc_show_outline);

    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);

    glUniform1i(m_show_outline_id, false);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void
NPC::updateShadowShaderModelMat(const glm::mat4& nodeTrans)
{
    m_shader->disable();
    m_shadow_shader.enable();

    //-- Set ModelView matrix:
    glUniformMatrix4fv(m_shadow_m_uni,
                       1,
                       GL_FALSE,
                       value_ptr(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(m_npc_shadow_shift_x,
                                                          m_npc_shadow_shift_y,
                                                          0.0f))
                                 * nodeTrans));
    m_shadow_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS;
}

void
NPC::updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans)
{
    m_shader->disable();
    m_shadow_shader.enable();

    //-- Set PV matrix:
    glUniformMatrix4fv(m_shadow_p_uni, 1, GL_FALSE, value_ptr(pTrans));
    glUniformMatrix4fv(m_shadow_v_uni, 1, GL_FALSE, value_ptr(vTrans));

    m_shadow_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS
}

void
NPC::translate(const glm::vec3& amount)
{
    m_npc_dx = m_npc_dx + amount.x;
    m_npc_dy = m_npc_dy + amount.y;

    SceneNode::translate(amount);
}

glm::vec4
NPC::getNPCCollideGeo()
{
    return glm::vec4(m_npc_dx,
                     m_npc_dy + m_npc_collide_y_offset,
                     SpriteSize::npcWidth,
                     m_npc_collide_height);
}

void
NPC::checkOnTop(glm::vec2 mousePos)
{
    if (m_npc_hover_x_offset + m_npc_dx < mousePos.x
        && mousePos.x < m_npc_dx + SpriteSize::npcWidth - m_npc_hover_x_offset) {
        if (m_npc_hover_y_offset + m_npc_dy < mousePos.y
            && mousePos.y < m_npc_dy + SpriteSize::npcHeight - m_npc_hover_y_offset) {
            m_npc_show_outline = true;
            return;
        }
    }

    m_npc_show_outline = false;
}
