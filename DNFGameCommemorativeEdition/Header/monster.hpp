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

/*
 * Layer One
 */
class Monster : public GeometryNode
{
public:
    Monster(ShaderProgram* shader);
    ~Monster() {}

    enum class MonsterMode { Stand, Walk, BasicAttack, Killed, ToBeDeleted };

    void setMonsterMode(MonsterMode mode);
    MonsterMode getMonsterMode();

    // Implement virtual function from base class
    // For monster, translate will not trigger walk mode
    // but, move monster will.
    void draw() override;
    const glm::mat4& getTransform() override;
    void translate(const glm::vec3& amount) override;

    // Need to use this for flocking since it will monitor the monster type
    void moveMonster(const glm::vec3& amount);

    // Map boundary set
    void setCurrentMapBoundary(glm::vec4 mapBoundary);

    glm::vec2 getMonsterCenter();
    float getMonsterDx();
    float getMonsterDy();

    // x, y -> x, y, x,y at bottom - left corner
    // z -> width
    // w -> height
    glm::vec4 getMonsterGeo();

    // x, y -> x, y, x,y at bottom - left corner with offset
    // z -> width
    // w -> height
    glm::vec4 getMonsterFloorObjCollideGeo();

    // Movement sign
    glm::vec2 getMonsterMovementAmount();

    // Update shadow shader matrix uniforms
    void updateShadowShaderModelMat(const glm::mat4& nodeTrans);
    void updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans);

    // Flip the direction of the sprite
    void flipSprite();
    bool spriteIsFacingLeft()
    {
        return m_monster_sprite_facing_left_dir;
    }

    // If the monster is attacking or killed, then do not handle movement.
    bool lockForMovement();

    // For flocking, here last trans is seemed as a move direction
    glm::vec3 getLastMonsterTrans()
    {
        return m_last_monster_trans;
    }

    void setLastMonsterTrans(const glm::vec3& trans)
    {
        m_last_monster_trans = trans;
    }

    // Check whether hit x or y map boundary and if it is the left or right, top or bottom boundary
    std::pair<std::pair<bool, bool>, std::pair<bool, bool>> checkHitMapBoundary(
        const glm::vec3& trans);

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

    // Uniforms and Attrib
    GLuint m_be_hit_id;
    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Monster Vertex Array Object
    GLuint m_monster_vao;
    // Monster Vertex Buffer Object
    GLuint m_monster_vbo;
    // Monster Texture Vertex Buffer Object
    GLuint m_monster_tex_vbo;

    GLfloat m_monster_width;
    GLfloat m_monster_height;

    // Animation
    // Animation speed here is divided by 1000
    bool m_monster_sprite_facing_left_dir;
    MonsterMode m_monster_mode;

    // Stand
    json m_monster_stand_json_parser;
    unsigned int m_number_of_stand_frames;
    float m_stand_animation_move_speed;
    std::string m_current_stand_frame;
    Texture m_stand_textures_sheet;

    // Killed
    json m_monster_killed_json_parser;
    unsigned int m_number_of_killed_frames;
    float m_killed_animation_move_speed;
    std::string m_current_killed_frame;
    Texture m_killed_textures_sheet;

    // Walk
    json m_monster_walk_json_parser;
    unsigned int m_number_of_walk_frames;
    float m_walk_animation_move_speed;
    std::string m_current_walk_frame;
    Texture m_walk_textures_sheet;

    // Override getTransform()
    // Due to a mistake for not separating the monster and the weapon,
    // we need to keep track of the scale of the frame texture geo change and m_trans together
    float m_current_scale_x;
    float m_current_scale_y;
    glm::mat4 m_return_trans;

    float m_animation_cursor;

    // Move and collision
    float m_monster_dx;
    float m_monster_dy;
    glm::vec2 m_monster_center;
    glm::vec4 m_current_map_boundary;

    // Record the last monster trans amount
    // in order to revert it for collision test succeeded
    glm::vec3 m_last_monster_trans;

    // Hit sound
    irrklang::ISoundSource* m_monster_be_hit;
};
