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
    void updateWindowGeo();

protected:
    ShaderProgram* m_shader;

    GLfloat m_window_width;
    GLfloat m_window_height;
};
