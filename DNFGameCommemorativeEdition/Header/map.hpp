#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"
#include "npc.hpp"

#include <glm.hpp>

class FloorObj;

class Map : public SceneNode
{
public:
    Map(ShaderProgram* shader, NPC* npc, GLfloat width, GLfloat height);
    ~Map();

    void initSceneOneMap();

    glm::vec4 getMapBoundary();
    const std::vector<std::pair<FloorObj*, glm::vec2>>& getFloorCollisionObjs();
    const std::vector<std::pair<FloorObj*, glm::vec2>>& getFloorReorderObjs();

protected:
    ShaderProgram* m_shader;

    NPC* m_npc;

    GLfloat m_window_width;
    GLfloat m_window_height;

    // Top, Bottom, x, y
    // Left, Right, z, w
    // Left starts at 0
    // Bottom starts at 0
    glm::vec4 m_map_boundary;
    float m_map_top_offset;
    float m_map_right_offset;

    // Use to detect collision and depth layer
    // Only layer two need to reorder depending on -y.
    // x, y -> x, y, x,y at bottom - left corner
    std::vector<FloorObj*> m_floor_obj_layer_one_list;
    std::vector<std::pair<FloorObj*, glm::vec2>> m_floor_obj_layer_two_list;
    std::vector<std::pair<FloorObj*, glm::vec2>> m_floor_obj_collision_list;
};
