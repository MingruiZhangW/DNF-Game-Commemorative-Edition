#pragma once

#include "dialog.hpp"
#include "dialogimage.hpp"
#include "dialogtextmanager.hpp"

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

    // Dialog will move along with camera
    void moveDialog(const glm::vec3& amount);

    void setCurrentDialogText(const std::string& dialogText);
    void setCurrentDialogSpeaker(const std::wstring& speakerName);

    void showDialogImage(bool show)
    {
        m_dialog_image->setShouldDraw(show);
    }

    void setShown(bool shown)
    {
        m_is_dialog_shown = shown;
    }

    bool getShown()
    {
        return m_is_dialog_shown;
    }

    void clearMovement();

private:
    void construct();

    ShaderProgram* m_shader;

    bool m_is_dialog_shown;

    std::unique_ptr<SceneNode> m_dialog_root_node;

    Dialog* m_dialog;
    DialogImage* m_dialog_image;
    std::unique_ptr<DialogTextManager> m_dialog_text_manager;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::string m_current_dialog_string;
    std::wstring m_current_dialog_speaker_string;
    std::vector<DialogChar*> m_current_dialog_text_chars;
    std::vector<DialogChar*> m_current_dialog_speaker_chars;
};
