#include "playerskilleffect.hpp"
#include "gamewindow.hpp"
#include "constant.hpp"
#include "player.hpp"

#include "glerrorcheck.hpp"

#include <fstream>

#include <gtc/matrix_transform.hpp>

// clang-format off

// PlayerSkillEffect vertex at the live area origin
static GLfloat player_skill_effect_vertex_buffer_data[] =
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

// PlayerSkillEffect texture coord
static GLfloat player_skill_effect_texture_coord_data[] =
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

// clang-format on

PlayerSkillEffect::PlayerSkillEffect(ShaderProgram* shader,
                                     ShaderProgram* shadowShader,
                                     Player* player)
    : GeometryNode(StringContant::playerSkillEffectName)
    , m_shader(shader)
    , m_shadow_shader(shadowShader)
    , m_player_skill_effect_dx(0.0f)
    , m_player_skill_effect_dy(0.0f)
    , m_current_play_skill_effect_frame("0")
    , m_player(player)
{
    // Load texture - stand
    std::ifstream ifs_se(TexturePath::playerSKillEffectJsonPath);
    m_play_skill_effect_json_parser = json::parse(ifs_se);
    m_number_of_skill_effect_frames = static_cast<unsigned int>(
        m_play_skill_effect_json_parser[SSJsonKeys::frames].size());
    m_textures_sheet = Texture(TexturePath::playerSKillEffectPNGPath);
    m_textures_sheet.loadTexture();

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_player_skill_effect_vao);
    glBindVertexArray(m_player_skill_effect_vao);

    // Create the floor vertex buffer
    glGenBuffers(1, &m_player_skill_effect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_skill_effect_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_skill_effect_vertex_buffer_data),
                 player_skill_effect_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Texture vbo
    glGenBuffers(1, &m_player_skill_effect_tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_skill_effect_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_skill_effect_texture_coord_data),
                 player_skill_effect_texture_coord_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_skill_effect_vbo);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(player_skill_effect_vertex_buffer_data[0]) * 3,
                          nullptr);

    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_skill_effect_tex_vbo);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(player_skill_effect_texture_coord_data[0]) * 2,
                          nullptr);

    updateTexCoord();

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
PlayerSkillEffect::useFrame(std::string frameNumber, bool flip)
{
    m_current_play_skill_effect_frame = frameNumber;
    m_player_sprite_facing_left_dir = flip;

    updateTexCoord();
}

void
PlayerSkillEffect::updateTexCoord()
{
    float texX, texY, texW, texH;

    // get sprite sheet coord
    texX = m_play_skill_effect_json_parser[SSJsonKeys::frames][m_current_play_skill_effect_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::x]
                                              .get<float>();
    texY = m_play_skill_effect_json_parser[SSJsonKeys::frames][m_current_play_skill_effect_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::y]
                                              .get<float>();
    texW = m_play_skill_effect_json_parser[SSJsonKeys::frames][m_current_play_skill_effect_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::w]
                                              .get<float>();
    texH = m_play_skill_effect_json_parser[SSJsonKeys::frames][m_current_play_skill_effect_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::h]
                                              .get<float>();

    // update each data point to tex coord
    player_skill_effect_texture_coord_data[0] = (texX + texW) / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[1] = (texY + texH) / m_textures_sheet.getTextureHeight();

    player_skill_effect_texture_coord_data[2] = texX / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[3] = (texY + texH) / m_textures_sheet.getTextureHeight();

    player_skill_effect_texture_coord_data[4] = (texX + texW) / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[5] = texY / m_textures_sheet.getTextureHeight();

    player_skill_effect_texture_coord_data[6] = texX / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[7] = texY / m_textures_sheet.getTextureHeight();

    player_skill_effect_texture_coord_data[8] = texX / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[9] = (texY + texH) / m_textures_sheet.getTextureHeight();

    player_skill_effect_texture_coord_data[10] = (texX + texW) / m_textures_sheet.getTextureWidth();
    player_skill_effect_texture_coord_data[11] = texY / m_textures_sheet.getTextureHeight();

    m_current_scale_x = texW;
    m_current_scale_y = texH;

    glBindVertexArray(m_player_skill_effect_vao);

    // texture coord
    glBindBuffer(GL_ARRAY_BUFFER, m_player_skill_effect_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_skill_effect_texture_coord_data),
                 player_skill_effect_texture_coord_data,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
PlayerSkillEffect::clearMovement()
{
    m_player_skill_effect_dx = 0.0f;
    m_player_skill_effect_dy = 0.0f;
    m_trans = glm::mat4(1.0f);
}

void
PlayerSkillEffect::draw()
{
    /* Draw shadow texture */
    m_shader->disable();
    m_shadow_shader->enable();

    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textures_sheet.useTexture();

    glBindVertexArray(m_player_skill_effect_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    m_shadow_shader->disable();
    m_shader->enable();

    /* Draw actual texture */

    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textures_sheet.useTexture();

    glBindVertexArray(m_player_skill_effect_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

const glm::mat4&
PlayerSkillEffect::getTransform()
{
    // S * T * R * T^-1
    // Scale will have no effect on translation
    // Initial setup
    glm::mat4 initTrans {1.0f};
    if (m_current_play_skill_effect_frame == "16" || m_current_play_skill_effect_frame == "17")
        initTrans = glm::translate(initTrans, glm::vec3(-219.0f, 0.0f, 0.0f));
    else if (m_current_play_skill_effect_frame == "18" || m_current_play_skill_effect_frame == "19")
        initTrans = glm::translate(initTrans, glm::vec3(-135.0f, 0.0f, 0.0f));

    initTrans = glm::scale(initTrans, glm::vec3(m_current_scale_x, m_current_scale_y, 1.0f));
    initTrans = glm::translate(initTrans, glm::vec3(0.5f, 0.5f, 0.0f));
    initTrans = glm::rotate(initTrans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    initTrans = glm::translate(initTrans, glm::vec3(-0.5f, -0.5f, 0.0f));

    if (!m_player_sprite_facing_left_dir) {
        initTrans = initTrans = initTrans
                                * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
        initTrans = initTrans
                    * glm::rotate(glm::mat4(1.0f),
                                  glm::radians(180.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f));
        initTrans = initTrans * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    }

    m_return_trans = glm::inverse(m_player->getTransform()) * m_trans * initTrans;

    return m_return_trans;
}
