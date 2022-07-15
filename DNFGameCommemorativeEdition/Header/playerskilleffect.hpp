#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

#include "json.hpp"

#include <glm.hpp>

using json = nlohmann::json;

class Player;

class PlayerSkillEffect : public GeometryNode
{
public:
    PlayerSkillEffect(ShaderProgram* shader, ShaderProgram* shadowShader, Player* player);
    ~PlayerSkillEffect() {}

    void draw() override;
    const glm::mat4& getTransform() override;

    void useFrame(std::string frameNumber, bool flip);

    void clearMovement();

private:
    void updateTexCoord();

    ShaderProgram* m_shader;
    ShaderProgram* m_shadow_shader;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    GLfloat m_player_skill_effect_dx;
    GLfloat m_player_skill_effect_dy;

    // Flip
    bool m_player_sprite_facing_left_dir;

    // Frame
    json m_play_skill_effect_json_parser;
    std::string m_current_play_skill_effect_frame;
    unsigned int m_number_of_skill_effect_frames;
    Texture m_textures_sheet;

    // Override getTransform()
    // Due to a mistake for not separating the player and the weapon,
    // we need to keep track of the scale of the frame texture geo change and m_trans together
    float m_current_scale_x;
    float m_current_scale_y;
    glm::mat4 m_return_trans;

    // PlayerSkillEffect Vertex Array Object
    GLuint m_player_skill_effect_vao;
    // PlayerSkillEffect Vertex Buffer Object
    GLuint m_player_skill_effect_vbo;
    // PlayerSkillEffect Texture Vertex Buffer Object
    GLuint m_player_skill_effect_tex_vbo;

    Player* m_player;
};
