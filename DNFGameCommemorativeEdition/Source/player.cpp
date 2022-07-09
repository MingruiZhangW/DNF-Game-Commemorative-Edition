#include "player.hpp"
#include "gamewindow.hpp"

#include "glerrorcheck.hpp"

#include <fstream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// clang-format off

// Player vertex at the live area origin
static GLfloat player_vertex_buffer_data[] =
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

// Player texture_coord
static GLfloat player_texture_coord_data[] =
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
static const float m_player_collide_width{60.0f};
static const float m_player_collide_height{30.0f};
static const float m_player_collide_x_offset{30.0f};

// Shadow shift const
static const float m_player_shadow_shift_x{80.0f};
static const float m_player_shadow_shift_y{-110.0f};

// clang-format on

Player::Player(ShaderProgram* shader)
    : GeometryNode(StringContant::playerName)
    , m_shader(shader)
    , m_stand_animation_move_speed(0.05f)
    , m_walk_animation_move_speed(0.05f)
    , m_player_mode(PlayerMode::Stand)
    , m_player_move_dir(PlayerMoveDir::None)
    , m_player_sprite_facing_left_dir(true)
    , m_current_stand_frame("0")
    , m_current_walk_frame("0")
    , m_animation_cursor(0.0f)
    , m_player_dx(0.0f)
    , m_player_dy(0.0f)
    , m_player_center(SpriteSize::playerWidth / 2.0f, SpriteSize::playerHeight / 2.0f)
    , m_current_map_boundary(glm::vec4(0.0f))
    , m_last_player_trans(glm::vec3(0.0f))
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
    std::ifstream ifs_s(TexturePath::playerStandJsonath);
    m_play_stand_json_parser = json::parse(ifs_s);
    m_number_of_stand_frames = static_cast<unsigned int>(
        m_play_stand_json_parser[SSJsonKeys::frames].size());
    m_stand_textures_sheet = Texture(TexturePath::playerStandPNGPath);
    m_stand_textures_sheet.loadTexture();

    // Load texture - walk
    std::ifstream ifs_w(TexturePath::playerWalkJsonPath);
    m_play_walk_json_parser = json::parse(ifs_w);
    m_number_of_walk_frames = static_cast<unsigned int>(
        m_play_walk_json_parser[SSJsonKeys::frames].size());
    m_walk_textures_sheet = Texture(TexturePath::playerWalkPNGPath);
    m_walk_textures_sheet.loadTexture();

    m_player_dx = m_player_center.x;
    m_player_dy = m_player_center.y;
    m_player_width = SpriteSize::playerWidth;
    m_player_height = SpriteSize::playerHeight;

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(m_player_width, m_player_height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_player_vao);
    glBindVertexArray(m_player_vao);

    // Create the floor vertex buffer
    glGenBuffers(1, &m_player_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_vertex_buffer_data),
                 player_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Texture vbo
    glGenBuffers(1, &m_player_tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_texture_coord_data),
                 player_texture_coord_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_vbo);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(player_vertex_buffer_data[0]) * 3,
                          nullptr);

    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_player_tex_vbo);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(player_texture_coord_data[0]) * 2,
                          nullptr);

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Player::updateFrame()
{
    m_animation_cursor = m_animation_cursor + GameWindow::getDeltaTime();

    switch (m_player_mode) {
    case Player::PlayerMode::Stand:
        if (m_animation_cursor > m_stand_animation_move_speed) {
            m_current_stand_frame = std::to_string((std::stoi(m_current_stand_frame) + 1)
                                                   % m_number_of_stand_frames);
            m_animation_cursor = 0;
        } else
            return;
        break;
    case Player::PlayerMode::Walk:
        if (m_animation_cursor > m_walk_animation_move_speed) {
            m_current_walk_frame = std::to_string((std::stoi(m_current_walk_frame) + 1)
                                                  % m_number_of_walk_frames);
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
Player::updateTexCoord()
{
    float texX, texY, texW, texH;

    switch (m_player_mode) {
    case Player::PlayerMode::Stand:
        // get sprite sheet coord
        texX = m_play_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                       [SSJsonKeys::frame][SSJsonKeys::x]
                                           .get<float>();
        texY = m_play_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                       [SSJsonKeys::frame][SSJsonKeys::y]
                                           .get<float>();
        texW = m_play_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                       [SSJsonKeys::frame][SSJsonKeys::w]
                                           .get<float>();
        texH = m_play_stand_json_parser[SSJsonKeys::frames][m_current_stand_frame]
                                       [SSJsonKeys::frame][SSJsonKeys::h]
                                           .get<float>();

        // update each data point to tex coord
        player_texture_coord_data[0] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[1] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        player_texture_coord_data[2] = texX / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[3] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        player_texture_coord_data[4] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[5] = texY / m_stand_textures_sheet.getTextureHeight();

        player_texture_coord_data[6] = texX / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[7] = texY / m_stand_textures_sheet.getTextureHeight();

        player_texture_coord_data[8] = texX / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[9] = (texY + texH) / m_stand_textures_sheet.getTextureHeight();

        player_texture_coord_data[10] = (texX + texW) / m_stand_textures_sheet.getTextureWidth();
        player_texture_coord_data[11] = texY / m_stand_textures_sheet.getTextureHeight();

        break;
    case Player::PlayerMode::Walk:
        // get sprite sheet coord
        texX = m_play_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::x]
                                          .get<float>();
        texY = m_play_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::y]
                                          .get<float>();
        texW = m_play_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::w]
                                          .get<float>();
        texH = m_play_walk_json_parser[SSJsonKeys::frames][m_current_walk_frame][SSJsonKeys::frame]
                                      [SSJsonKeys::h]
                                          .get<float>();

        // update each data point to tex coord
        player_texture_coord_data[0] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[1] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        player_texture_coord_data[2] = texX / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[3] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        player_texture_coord_data[4] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[5] = texY / m_walk_textures_sheet.getTextureHeight();

        player_texture_coord_data[6] = texX / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[7] = texY / m_walk_textures_sheet.getTextureHeight();

        player_texture_coord_data[8] = texX / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[9] = (texY + texH) / m_walk_textures_sheet.getTextureHeight();

        player_texture_coord_data[10] = (texX + texW) / m_walk_textures_sheet.getTextureWidth();
        player_texture_coord_data[11] = texY / m_walk_textures_sheet.getTextureHeight();
        break;
    default:
        break;
    }

    glBindVertexArray(m_player_vao);

    // texture coord
    glBindBuffer(GL_ARRAY_BUFFER, m_player_tex_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(player_texture_coord_data),
                 player_texture_coord_data,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
Player::draw()
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

    switch (m_player_mode) {
    case PlayerMode::Stand:
        m_stand_textures_sheet.useTexture();
        break;
    case PlayerMode::Walk:
        m_walk_textures_sheet.useTexture();
        break;
    default:
        break;
    }

    glBindVertexArray(m_player_vao);
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

    switch (m_player_mode) {
    case PlayerMode::Stand:
        m_stand_textures_sheet.useTexture();
        break;
    case PlayerMode::Walk:
        m_walk_textures_sheet.useTexture();
        break;
    default:
        break;
    }

    glBindVertexArray(m_player_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void
Player::move(PlayerMoveDir playerMoveDir)
{
    switch (playerMoveDir) {
    case Player::None:
        m_last_player_trans = glm::vec3(0.0f);
        break;
    case Player::Left:
        if (!m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(-PlayerFollowCameraSpeed::walk, 0.0, 0.0f);
        break;
    case Player::LeftUp:
        if (!m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(-PlayerFollowCameraSpeed::walk,
                                        PlayerFollowCameraSpeed::walk,
                                        0.0f);
        break;
    case Player::LeftDown:
        if (!m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(-PlayerFollowCameraSpeed::walk,
                                        -PlayerFollowCameraSpeed::walk,
                                        0.0f);
        break;
    case Player::Right:
        if (m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(PlayerFollowCameraSpeed::walk, 0.0f, 0.0f);
        break;
    case Player::RightUp:
        if (m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(PlayerFollowCameraSpeed::walk,
                                        PlayerFollowCameraSpeed::walk,
                                        0.0f);
        break;
    case Player::RightDown:
        if (m_player_sprite_facing_left_dir)
            flipSprite();

        m_last_player_trans = glm::vec3(PlayerFollowCameraSpeed::walk,
                                        -PlayerFollowCameraSpeed::walk,
                                        0.0f);
        break;
    case Player::Up:
        m_last_player_trans = glm::vec3(0.0f, PlayerFollowCameraSpeed::walk, 0.0f);
        break;
    case Player::Down:
        m_last_player_trans = glm::vec3(0.0f, -PlayerFollowCameraSpeed::walk, 0.0f);
        break;
    default:
        break;
    }

    translate(m_last_player_trans);
}

void
Player::flipSprite()
{
    m_player_sprite_facing_left_dir = !m_player_sprite_facing_left_dir;

    m_trans = m_trans = m_trans * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = m_trans
              * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_trans = m_trans * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, 0.0f));
}

void
Player::translate(const glm::vec3& amount)
{
    auto finalTrans = amount;
    if (m_player_dx + amount.x < m_current_map_boundary.z
        || m_player_dx + amount.x > m_current_map_boundary.w)
        finalTrans.x = 0.0f;
    else
        m_player_dx = m_player_dx + finalTrans.x;

    if (m_player_dy + amount.y > m_current_map_boundary.x
        || m_player_dy + amount.y < m_current_map_boundary.y)
        finalTrans.y = 0.0f;
    else
        m_player_dy = m_player_dy + finalTrans.y;

    SceneNode::translate(finalTrans);
}

void
Player::setCurrentMapBoundary(glm::vec4 mapBoundary)
{
    m_current_map_boundary = mapBoundary;
    m_current_map_boundary.x = m_current_map_boundary.x;
    m_current_map_boundary.y = m_current_map_boundary.y + m_player_center.y;

    m_current_map_boundary.z = m_current_map_boundary.z + m_player_center.x;
    m_current_map_boundary.w = m_current_map_boundary.w - m_player_center.x;
}

void
Player::reverseMove(std::pair<bool, bool> dirToReverse)
{
    auto toReverseMove = glm::vec3(dirToReverse.first ? -m_last_player_trans.x : 0.0f,
                                   dirToReverse.second ? -m_last_player_trans.y : 0.0f,
                                   0.0f);
    translate(toReverseMove);
}

float
Player::getPlayerDx()
{
    return m_player_dx;
}

float
Player::getPlayerDy()
{
    return m_player_dy;
}

glm::vec2
Player::getPlayerCenter()
{
    return m_player_center;
}

glm::vec4
Player::getPlayerGeo()
{
    return glm::vec4(m_player_dx - m_player_center.x,
                     m_player_dy - m_player_center.y,
                     m_player_width,
                     m_player_height);
}

glm::vec4
Player::getPlayerFloorObjCollideGeo()
{
    return glm::vec4(m_player_dx - m_player_collide_x_offset,
                     m_player_dy - m_player_center.y,
                     m_player_collide_width,
                     m_player_collide_height);
}

glm::vec2
Player::getPlayerMovementAmount()
{
    return glm::vec2(m_last_player_trans.x, m_last_player_trans.y);
}

void
Player::updateShadowShaderModelMat(const glm::mat4& nodeTrans)
{
    m_shader->disable();
    m_shadow_shader.enable();

    //-- Set ModelView matrix:
    glUniformMatrix4fv(m_shadow_m_uni,
                       1,
                       GL_FALSE,
                       value_ptr(glm::translate(glm::mat4(1.0f),
                                                glm::vec3(m_player_sprite_facing_left_dir
                                                              ? m_player_shadow_shift_x
                                                              : -m_player_shadow_shift_x,
                                                          m_player_shadow_shift_y,
                                                          0.0f))
                                 * nodeTrans));
    m_shadow_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS;
}

void
Player::updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans)
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
Player::setPlayerMode(PlayerMode mode)
{
    if (m_player_mode == mode)
        return;

    m_player_mode = mode;

    m_current_walk_frame = "0";
    m_current_stand_frame = "0";
    m_animation_cursor = 0;

    updateTexCoord();
}
