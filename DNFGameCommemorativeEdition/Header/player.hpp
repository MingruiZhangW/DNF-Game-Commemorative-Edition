#pragma once

#include "geometrynode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"

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

    void draw();

private:
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

    PlayerMode m_player_mode;

    json m_play_stand_json_parser;
    unsigned int m_number_of_stand_frames;
    unsigned int m_stand_move_speed;
    std::string m_current_stand_frame;
    Texture m_stand_textures_sheet;

    json m_play_walk_json_parser;
    unsigned int m_number_of_walk_frames;
    unsigned int m_walk_move_speed;
    std::string m_current_walk_frame;
    Texture m_walk_textures_sheet;
};
