#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"

#include <glm.hpp>

class FloorObj;

class Map : public SceneNode
{
public:
    Map(ShaderProgram* shader, GLfloat width, GLfloat height);
    ~Map();

    void initSceneOneMap();

    glm::vec4 getMapBoundary();
    const std::vector<std::pair<FloorObj*, glm::vec2>>& getFloorObjs();

protected:
    ShaderProgram* m_shader;

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
    // x, y -> x, y, x,y at bottom - left corner
    std::vector<std::pair<FloorObj*, glm::vec2>> m_floor_obj_list;
};
