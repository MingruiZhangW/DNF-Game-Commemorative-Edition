#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"
#include "npc.hpp"

#include <glm.hpp>

class FloorObj;

/*
 * Layer Zero
 */
class Map : public SceneNode
{
public:
    Map(ShaderProgram* shader, NPC* npc, GLfloat width, GLfloat height);
    ~Map();

    // Two kind of map
    void initSceneOneMap();
    void initSceneTwoMap();

    // Map boundary to limit the player and monster movements
    glm::vec4 getMapBoundary();

    // Objs that need to do collisions detections
    const std::vector<std::pair<FloorObj*, glm::vec2>>& getFloorCollisionObjs();

    // Objs that need to rearrange the drawing order
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
    // Here layer one represent no need to rearrange the drawing order
    // layer two means we need to rearrange the drawing order
    std::vector<FloorObj*> m_floor_obj_layer_one_list;
    std::vector<std::pair<FloorObj*, glm::vec2>> m_floor_obj_layer_two_list;
    std::vector<std::pair<FloorObj*, glm::vec2>> m_floor_obj_collision_list;
};
