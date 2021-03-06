#include "monster.hpp"
#include "gamewindow.hpp"
#include "game.hpp"

#include "glerrorcheck.hpp"

#include <fstream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// clang-format off

// Monster vertex at the live area origin
static GLfloat monster_vertex_buffer_data[] =
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

// Monster texture_coord
static GLfloat monster_texture_coord_data[] =
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

// Collide const
static const float m_monster_collide_width{60.0f};
static const float m_monster_collide_height{30.0f};
static const float m_monster_collide_x_offset{30.0f};

// Shadow shift const
static const float m_monster_shadow_shift_x{40.0f};
static const float m_monster_shadow_shift_y{-105.0f};

// clang-format on

Monster::Monster(ShaderProgram* shader)
    : GeometryNode(StringContant::monsterName)
    , m_shader(shader)
    , m_stand_animation_move_speed(0.05f)
    , m_walk_animation_move_speed(0.05f)
    , m_killed_animation_move_speed(0.02f)
    , m_monster_mode(MonsterMode::Stand)
    , m_monster_sprite_facing_left_dir(true)
    , m_current_stand_frame("0")
    , m_current_killed_frame("0")
    , m_current_walk_frame("0")
    , m_animation_cursor(0.0f)
    , m_monster_dx(0.0f)
    , m_monster_dy(0.0f)
    , m_monster_center(SpriteSize::monsterWidth / 2.0f, SpriteSize::monsterHeight / 2.0f)
    , m_current_map_boundary(glm::vec4(0.0f))
    , m_last_monster_trans(glm::vec3(0.0f))
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
    std::ifstream ifs_s(TexturePath::monsterStandJsonPath);
    m_monster_stand_json_parser = json::parse(ifs_s);
    m_number_of_stand_frames = static_cast<unsigned int>(
        m_monster_stand_json_parser[SSJsonKeys::frames].size());
    m_stand_textures_sheet = Texture(TexturePath::monsterStandPNGPath);
    m_stand_textures_sheet.loadTexture();

    // Load texture - killed
    std::ifstream ifs_k(TexturePath::monsterKilledJsonPath);
    m_monster_killed_json_parser = json::parse(ifs_k);
    m_number_of_killed_frames = static_cast<unsigned int>(
        m_monster_killed_json_parser[SSJsonKeys::frames].size());
    m_killed_textures_sheet = Texture(TexturePath::monsterKilledPNGPath);
    m_killed_textures_sheet.loadTexture();

    // Load texture - walk
    std::ifstream ifs_w(TexturePath::monsterWalkJsonPath);
    m_monster_walk_json_parser = json::parse(ifs_w);
    m_number_of_walk_frames = static_cast<unsigned int>(
        m_monster_walk_json_parser[SSJsonKeys::frames].size());
    m_walk_textures_sheet = Texture(TexturePath::monsterWalkPNGPath);
    m_walk_textures_sheet.loadTexture();

    m_monster_dx = m_monster_center.x;
    m_monster_dy = m_monster_center.y;
    m_monster_width = SpriteSize::monsterWidth;
    m_monster_height = SpriteSize::monsterHeight;
    m_current_scale_x = m_monster_width;
    m_current_scale_y = m_monster_height;

    // Create the vertex array to record buffer assignments for monster.
    glGenVertexArrays(1, &m_monster_vao);
    glBindVertexArray(m_monster_vao);

    // Create the monster vertex buffer
    glGenBuffers(1, &m_monster_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_monster_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(monster_vertex_buffer_data),
                 monster_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Texture vbo
    // GL_DYNAMIC_DRAW since we need to update it constantly
    glGenBuffers(1, &m_monster_tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_monster_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(monster_texture_coord_data),
                 monster_texture_coord_data,
                 GL_DYNAMIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_monster_vbo);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(monster_vertex_buffer_data[0]) * 3,
                          nullptr);

    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_monster_tex_vbo);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(monster_texture_coord_data[0]) * 2,
                          nullptr);

    m_be_hit_id = m_shader->getUniformLocation("hit");

    updateTexCoord();

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;

    // Init sound
    m_monster_be_hit = Game::getSoundEngine()->getSoundSource(SoundPath::monsterBeHit.c_str(), true);
}

void
Monster::updateFrame()
{
    m_animation_cursor = m_animation_cursor + GameWindow::getDeltaTime();

    switch (m_monster_mode) {
    case Monster::MonsterMode::Stand:
        if (m_animation_cursor > m_stand_animation_move_speed) {
            m_current_stand_frame = std::to_string((std::stoi(m_current_stand_frame) + 1)
                                                   % m_number_of_stand_frames);
            m_animation_cursor = 0;
        } else
            return;
        break;
    case Monster::MonsterMode::Walk:
        if (m_animation_cursor > m_walk_animation_move_speed) {
            m_current_walk_frame = std::to_string((std::stoi(m_current_walk_frame) + 1)
                                                  % m_number_of_walk_frames);
            m_animation_cursor = 0;
        } else
            return;
        break;
    case Monster::MonsterMode::Killed:
        if (m_animation_cursor > m_killed_animation_move_speed) {
            m_current_killed_frame = std::to_string((std::stoi(m_current_killed_frame) + 1)
                                                    % m_number_of_killed_frames);
            m_animation_cursor = 0;
        } else
            return;
        break;
    default:
        break;
    }

    updateTexCoord();
}

void
Monster::updateTexCoord()
{
    float texX, texY, texW, texH;

    switch (m_monster_mode) {
    case Monster::MonsterMode::Stand:
        // get sprite sheet coord
        texX = m_monster_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::x]
                                              .get<float>();
        texY = m_monster_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::y]
                                              .get<float>();
        texW = m_monster_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::w]
                                              .get<float>();
        texH = m_monster_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                          [SSJsonKeys::frame][SSJsonKeys::h]
                                              .get<float>();

        // update each data point to tex coord
        monster_texture_coord_data[0] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[1] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        monster_texture_coord_data[2] = texX / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[3] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        monster_texture_coord_data[4] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[5] = texY / m_stand_textures_sheet.getTextureHeight();

        monster_texture_coord_data[6] = texX / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[7] = texY / m_stand_textures_sheet.getTextureHeight();

        monster_texture_coord_data[8] = texX / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[9] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        monster_texture_coord_data[10] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        monster_texture_coord_data[11] = texY / m_stand_textures_sheet.getTextureHeight();

        break;
    case Monster::MonsterMode::Walk:
        // get sprite sheet coord
        texX = m_monster_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame]
                                         [SSJsonKeys::frame][SSJsonKeys::x]
                                             .get<float>();
        texY = m_monster_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame]
                                         [SSJsonKeys::frame][SSJsonKeys::y]
                                             .get<float>();
        texW = m_monster_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame]
                                         [SSJsonKeys::frame][SSJsonKeys::w]
                                             .get<float>();
        texH = m_monster_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame]
                                         [SSJsonKeys::frame][SSJsonKeys::h]
                                             .get<float>();

        // update each data point to tex coord
        monster_texture_coord_data[0] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[1] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        monster_texture_coord_data[2] = texX / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[3] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        monster_texture_coord_data[4] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[5] = texY / m_walk_textures_sheet.getTextureHeight();

        monster_texture_coord_data[6] = texX / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[7] = texY / m_walk_textures_sheet.getTextureHeight();

        monster_texture_coord_data[8] = texX / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[9] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        monster_texture_coord_data[10] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        monster_texture_coord_data[11] = texY / m_walk_textures_sheet.getTextureHeight();

        break;
    case Monster::MonsterMode::Killed:
        // get sprite sheet coord
        texX = m_monster_killed_json_parser[SSJsonKeys::frames][m_current_killed_frame]
                                           [SSJsonKeys::frame][SSJsonKeys::x]
                                               .get<float>();
        texY = m_monster_killed_json_parser[SSJsonKeys::frames][m_current_killed_frame]
                                           [SSJsonKeys::frame][SSJsonKeys::y]
                                               .get<float>();
        texW = m_monster_killed_json_parser[SSJsonKeys::frames][m_current_killed_frame]
                                           [SSJsonKeys::frame][SSJsonKeys::w]
                                               .get<float>();
        texH = m_monster_killed_json_parser[SSJsonKeys::frames][m_current_killed_frame]
                                           [SSJsonKeys::frame][SSJsonKeys::h]
                                               .get<float>();

        // update each data point to tex coord
        monster_texture_coord_data[0] = (texX + texW) / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[1] = (texY + texH) / m_killed_textures_sheet.getTextureHeight();

        monster_texture_coord_data[2] = texX / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[3] = (texY + texH) / m_killed_textures_sheet.getTextureHeight();

        monster_texture_coord_data[4] = (texX + texW) / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[5] = texY / m_killed_textures_sheet.getTextureHeight();

        monster_texture_coord_data[6] = texX / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[7] = texY / m_killed_textures_sheet.getTextureHeight();

        monster_texture_coord_data[8] = texX / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[9] = (texY + texH) / m_killed_textures_sheet.getTextureHeight();

        monster_texture_coord_data[10] = (texX + texW) / m_killed_textures_sheet.getTextureWidth();
        monster_texture_coord_data[11] = texY / m_killed_textures_sheet.getTextureHeight();

        break;
    default:
        break;
    }

    m_current_scale_x = texW;
    m_current_scale_y = texH;

    glBindVertexArray(m_monster_vao);

    // texture coord
    // GL_DYNAMIC_DRAW use glBufferSubData
    glBindBuffer(GL_ARRAY_BUFFER, m_monster_tex_vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(monster_texture_coord_data),
                    monster_texture_coord_data);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Monster::draw()
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

    switch (m_monster_mode) {
    case MonsterMode::Stand:
        m_stand_textures_sheet.useTexture();
        break;
    case MonsterMode::Walk:
        m_walk_textures_sheet.useTexture();
        break;
    case MonsterMode::Killed:
        m_killed_textures_sheet.useTexture();
        break;
    default:
        break;
    }

    glBindVertexArray(m_monster_vao);
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

    switch (m_monster_mode) {
    case MonsterMode::Stand:
        m_stand_textures_sheet.useTexture();
        break;
    case MonsterMode::Walk:
        m_walk_textures_sheet.useTexture();
        break;
    case MonsterMode::Killed:
        glUniform1i(m_be_hit_id, true);

        m_killed_textures_sheet.useTexture();
        break;
    default:
        break;
    }

    glBindVertexArray(m_monster_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glUniform1i(m_be_hit_id, false);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    afterDraw();
}

void
Monster::afterDraw()
{
    if (m_monster_mode == MonsterMode::Killed && m_current_killed_frame == "1")
        Game::getSoundEngine()->play2D(m_monster_be_hit, false, false, true);
    if (m_monster_mode == MonsterMode::Killed
        && std::stoi(m_current_killed_frame) + 1 == m_number_of_killed_frames)
        setMonsterMode(MonsterMode::ToBeDeleted);
}

void
Monster::flipSprite()
{
    m_monster_sprite_facing_left_dir = !m_monster_sprite_facing_left_dir;
}

void
Monster::moveMonster(const glm::vec3& amount)
{
    if (abs(amount.x) < 0.1f && abs(amount.y) < 0.1f)
        m_monster_mode = MonsterMode::Stand;
    else
        m_monster_mode = MonsterMode::Walk;

    translate(amount);
}

void
Monster::translate(const glm::vec3& amount)
{
    auto finalTrans = amount;
    if (m_monster_dx + amount.x < m_current_map_boundary.z
        || m_monster_dx + amount.x > m_current_map_boundary.w)
        finalTrans.x = 0.0f;
    else
        m_monster_dx = m_monster_dx + finalTrans.x;

    if (m_monster_dy + amount.y > m_current_map_boundary.x
        || m_monster_dy + amount.y < m_current_map_boundary.y)
        finalTrans.y = 0.0f;
    else
        m_monster_dy = m_monster_dy + finalTrans.y;

    m_last_monster_trans = finalTrans;

    SceneNode::translate(finalTrans);
}

std::pair<std::pair<bool, bool>, std::pair<bool, bool>>
Monster::checkHitMapBoundary(const glm::vec3& trans)
{
    std::pair<std::pair<bool, bool>, std::pair<bool, bool>> result {{false, false}, {false, false}};

    if (m_monster_dx + trans.x < m_current_map_boundary.z) {
        // x, left boundary
        result.first.first = true;
        result.first.second = true;
    } else if (m_monster_dx + trans.x > m_current_map_boundary.w) {
        // x, right boundary
        result.first.first = true;
        result.first.second = false;
    }

    if (m_monster_dy + trans.y > m_current_map_boundary.x) {
        // y, top boundary
        result.second.first = true;
        result.second.second = true;
    } else if (m_monster_dy + trans.y < m_current_map_boundary.y) {
        // y, bottom boundary
        result.second.first = true;
        result.second.second = false;
    }

    return result;
}

void
Monster::setCurrentMapBoundary(glm::vec4 mapBoundary)
{
    m_current_map_boundary = mapBoundary;
    m_current_map_boundary.x = m_current_map_boundary.x;
    m_current_map_boundary.y = m_current_map_boundary.y + m_monster_center.y;

    m_current_map_boundary.z = m_current_map_boundary.z + m_monster_center.x;
    m_current_map_boundary.w = m_current_map_boundary.w - m_monster_center.x;
}

float
Monster::getMonsterDx()
{
    return m_monster_dx;
}

float
Monster::getMonsterDy()
{
    return m_monster_dy;
}

glm::vec2
Monster::getMonsterCenter()
{
    return m_monster_center;
}

glm::vec4
Monster::getMonsterGeo()
{
    return glm::vec4(m_monster_dx - m_monster_center.x,
                     m_monster_dy - m_monster_center.y,
                     m_monster_width,
                     m_monster_height);
}

glm::vec4
Monster::getMonsterFloorObjCollideGeo()
{
    return glm::vec4(m_monster_dx - m_monster_collide_x_offset,
                     m_monster_dy - m_monster_center.y,
                     m_monster_collide_width,
                     m_monster_collide_height);
}

glm::vec2
Monster::getMonsterMovementAmount()
{
    return glm::vec2(m_last_monster_trans.x, m_last_monster_trans.y);
}

void
Monster::updateShadowShaderModelMat(const glm::mat4& nodeTrans)
{
    m_shader->disable();
    m_shadow_shader.enable();

    //-- Set ModelView matrix:
    glUniformMatrix4fv(m_shadow_m_uni,
                       1,
                       GL_FALSE,
                       value_ptr(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(m_monster_sprite_facing_left_dir
                                                              ? m_monster_shadow_shift_x
                                                              : -m_monster_shadow_shift_x,
                                                          m_monster_shadow_shift_y,
                                                          0.0f))
                                 * nodeTrans));
    m_shadow_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS;
}

void
Monster::updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans)
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
Monster::setMonsterMode(MonsterMode mode)
{
    if (m_monster_mode == mode)
        return;

    m_monster_mode = mode;

    m_current_stand_frame = "0";
    m_current_killed_frame = "0";
    m_current_walk_frame = "0";
    m_animation_cursor = 0;

    updateTexCoord();
}

Monster::MonsterMode
Monster::getMonsterMode()
{
    return m_monster_mode;
}

bool
Monster::lockForMovement()
{
    if (m_monster_mode == MonsterMode::BasicAttack || m_monster_mode == MonsterMode::Killed)
        return true;
    return false;
}

const glm::mat4&
Monster::getTransform()
{
    // S * T * R * T^-1
    // Scale will have no effect on translation
    // Initial setup
    auto initTrans = glm::scale(glm::mat4(1.0f),
                                glm::vec3(m_current_scale_x, m_current_scale_y, 1.0f));
    initTrans = glm::translate(initTrans, glm::vec3(0.5f, 0.5f, 0.0f));
    initTrans = glm::rotate(initTrans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    initTrans = glm::translate(initTrans, glm::vec3(-0.5f, -0.5f, 0.0f));

    if (!m_monster_sprite_facing_left_dir) {
        initTrans = initTrans = initTrans
                                * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
        initTrans = initTrans
                    * glm::rotate(glm::mat4(1.0f),
                                  glm::radians(180.0f),
                                  glm::vec3(0.0f, 1.0f, 0.0f));
        initTrans = initTrans * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
    }

    m_return_trans = m_trans * initTrans;

    return m_return_trans;
}
