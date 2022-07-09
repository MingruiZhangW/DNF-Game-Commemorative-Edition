#pragma once

#include "geometrynode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"
#include "constant.hpp"

#include "json.hpp"

#include <glm.hpp>

#include <vector>

using json = nlohmann::json;

class Player : public GeometryNode
{
public:
    Player(ShaderProgram* shader);
    ~Player() {}

    enum class PlayerMode { Stand, Walk };

    // Make enum instead of enum class
    enum PlayerMoveDir {
        None = 0,
        Left = GLFWArrowKeyRemap::leftKey,
        LeftUp = GLFWArrowKeyRemap::leftKey + GLFWArrowKeyRemap::upKey,
        LeftDown = GLFWArrowKeyRemap::leftKey + GLFWArrowKeyRemap::downKey,
        Right = GLFWArrowKeyRemap::rightKey,
        RightUp = GLFWArrowKeyRemap::rightKey + GLFWArrowKeyRemap::upKey,
        RightDown = GLFWArrowKeyRemap::rightKey + GLFWArrowKeyRemap::downKey,
        Up = GLFWArrowKeyRemap::upKey,
        Down = GLFWArrowKeyRemap::downKey
    };

    // Implement virtual function from base class
    void draw();
    void translate(const glm::vec3& amount);

    void move(PlayerMoveDir playerMoveDir);
    void reverseMove(std::pair<bool, bool> dirToReverse);
    void setCurrentMapBoundary(glm::vec4 mapBoundary);

    glm::vec2 getPlayerCenter();
    float getPlayerDx();
    float getPlayerDy();

    // x, y -> x, y, x,y at bottom - left corner
    // z -> width
    // w -> height
    glm::vec4 getPlayerGeo();

    // x, y -> x, y, x,y at bottom - left corner with offset
    // z -> width - offset * 2
    glm::vec4 getPlayerFloorObjCollideGeo();

    // Movement sign
    glm::vec2 getPlayerMovementAmount();

private:
    void flipSprite();
    void updateTexCoord();

    ShaderProgram* m_shader;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Player Vertex Array Object
    GLuint m_player_vao;
    // Player Vertex Buffer Object
    GLuint m_player_vbo;
    // Player Texture Vertex Buffer Object
    GLuint m_player_tex_vbo;

    GLfloat m_player_width;
    GLfloat m_player_height;

    bool m_player_sprite_facing_left_dir;
    PlayerMode m_player_mode;
    PlayerMoveDir m_player_move_dir;

    json m_play_stand_json_parser;
    unsigned int m_number_of_stand_frames;
    unsigned int m_stand_animation_move_speed;
    std::string m_current_stand_frame;
    Texture m_stand_textures_sheet;

    json m_play_walk_json_parser;
    unsigned int m_number_of_walk_frames;
    unsigned int m_walk_animation_move_speed;
    std::string m_current_walk_frame;
    Texture m_walk_textures_sheet;

    float m_player_dx;
    float m_player_dy;
    glm::vec2 m_player_center;
    glm::vec4 m_current_map_boundary;

    // Record the last player trans amount
    // in order to revert it for collision test succeeded
    glm::vec3 m_last_player_trans;
};
