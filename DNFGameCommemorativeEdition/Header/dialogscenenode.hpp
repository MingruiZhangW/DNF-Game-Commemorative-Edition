#pragma once

#include "dialog.hpp"
#include "dialogimage.hpp"

#include <memory>

class DialogSceneNode
{
public:
    DialogSceneNode(ShaderProgram* shader, GLfloat frameBufferWidth, GLfloat frameBufferHeight);

    ~DialogSceneNode();

    SceneNode* getRoot()
    {
        return m_dialog_root_node.get();
    }

    void moveDialog(const glm::vec3& amount);

private:
    void construct();

    ShaderProgram* m_shader;

    std::unique_ptr<SceneNode> m_dialog_root_node;

    Dialog* m_dialog;
    DialogImage* m_dialog_image;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;
};
