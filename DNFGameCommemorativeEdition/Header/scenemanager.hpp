#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"

#include "GL/glew.h"

#include <memory>

class SceneManager
{
public:
    SceneManager(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight);
    ~SceneManager();

    void constructScenes();

    void drawSceneOne();

private:
    void constructSceneOne();
    void renderSceneGraphNodes(SceneNode* node, glm::mat4 modelMat);

    ShaderProgram* m_shader;
    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::unique_ptr<SceneNode> m_scene_one_root_node;
};
