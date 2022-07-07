#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"

#include <glm.hpp>

class Map : public SceneNode
{
public:
    Map(ShaderProgram* shader, GLfloat width, GLfloat height);
    ~Map();

    void initSceneOneMap();

    glm::vec4 getMapBoundary();

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
};
