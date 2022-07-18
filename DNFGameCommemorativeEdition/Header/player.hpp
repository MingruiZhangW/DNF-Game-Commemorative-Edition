#pragma once

#include "geometrynode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"
#include "constant.hpp"

#include "json.hpp"

#include <irrKlang.h>
#include <glm.hpp>

#include <vector>

using json = nlohmann::json;

class PlayerSkillEffect;

class Player : public GeometryNode
{
public:
    Player(ShaderProgram* shader);
    ~Player() {}

    enum class PlayerMode { Stand, Walk, BasicAttack, Skill };

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

    void setPlayerMode(PlayerMode mode);
    PlayerMode getPlayerMode();

    // Implement virtual function from base class
    void draw() override;
    void translate(const glm::vec3& amount) override;
    const glm::mat4& getTransform() override;

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
    // z -> width
    // w -> height
    glm::vec4 getPlayerFloorObjCollideGeo();
    glm::vec4 getPlayerAttackCollideGeo();

    // Movement sign
    glm::vec2 getPlayerMovementAmount();

    // Update shadow shader matrix uniforms
    void updateShadowShaderModelMat(const glm::mat4& nodeTrans);
    void updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans);

    // Flip the direction of the sprite
    void flipSprite();

    // If the player is attacking, then do not handle movement.
    bool lockForMovement();

    // Be ready for scene transition
    void cleanMovement();

private:
    void afterDraw();
    void updateTexCoord();
    void updateFrame();

    ShaderProgram* m_shader;

    // Uniform location for PVM matrix for shadow shader.
    // Shadow is created by shear transformation and assign black/alpha value
    // in fragement shader
    GLint m_shadow_p_uni;
    GLint m_shadow_v_uni;
    GLint m_shadow_m_uni;
    ShaderProgram m_shadow_shader;

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

    // Animation
    // Animation speed here is divided by 1000
    bool m_player_sprite_facing_left_dir;
    PlayerMode m_player_mode;
    PlayerMoveDir m_player_move_dir;

    // Stand
    json m_play_stand_json_parser;
    unsigned int m_number_of_stand_frames;
    float m_stand_animation_move_speed;
    std::string m_current_stand_frame;
    Texture m_stand_textures_sheet;

    // Walk
    json m_play_walk_json_parser;
    unsigned int m_number_of_walk_frames;
    float m_walk_animation_move_speed;
    std::string m_current_walk_frame;
    Texture m_walk_textures_sheet;

    // Basic Attack
    json m_play_basic_attack_json_parser;
    unsigned int m_number_of_basic_attack_frames;
    float m_basic_attack_animation_move_speed;
    std::string m_current_basic_attack_frame;
    Texture m_basic_attack_textures_sheet;

    // Skill
    json m_play_skill_json_parser;
    unsigned int m_number_of_skill_frames;
    float m_skill_animation_move_speed;
    std::string m_current_skill_frame;
    Texture m_skill_textures_sheet;

    // Override getTransform()
    // Due to a mistake for not separating the player and the weapon,
    // we need to keep track of the scale of the frame texture geo change and m_trans together
    float m_current_scale_x;
    float m_current_scale_y;
    glm::mat4 m_return_trans;

    float m_animation_cursor;

    // Move and collision
    float m_player_dx;
    float m_player_dy;
    glm::vec2 m_player_center;
    glm::vec4 m_current_map_boundary;

    // Record the last player trans amount
    // in order to revert it for collision test succeeded
    glm::vec3 m_last_player_trans;

    // Skill effect
    PlayerSkillEffect* m_player_skill_effect;

    // Cut Sound
    irrklang::ISound* m_player_attack_sound;
    irrklang::ISoundSource* m_player_attack;
};
