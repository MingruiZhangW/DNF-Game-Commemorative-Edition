#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"

#include "GL/glew.h"

#include <memory>

class Map;
class Player;
class NPC;
class DialogSceneNode;

class SceneOne
{
public:
    // TODO: add finite state machine.
    enum class DialogConvMode {
        ConvOne,
        ConvTwo,
        ConvThree,
        ConvFour,
        ConvFive,
        ConvSix,
        ConvSeven,
        ConvEight
    };

    SceneOne(ShaderProgram* shader,
             GLfloat frameBufferWidth,
             GLfloat frameBufferHeight,
             Player* player,
             NPC* npc,
             DialogSceneNode* dialogSceneNode);

    ~SceneOne();

    SceneNode* getRootSceneNode()
    {
        return m_scene_one_root_node.get();
    }

    glm::vec4 getSceneOneMapBoundary()
    {
        return m_scene_one_map_boundary;
    }

    // Return vertical or horizontal collision and whether the door is hit.
    std::pair<bool, std::pair<bool, bool>> sceneOneCollisionTest(const glm::vec2& movement);

    // Layered drawing
    void reorderLayerNodeChild();

    // NPC click, hover event handle
    bool processHover(const glm::vec2& mousePos);
    bool processClick();

    // Dialog
    void moveDialog(float dx);

    // Initial display, should call first.
    void prepareInitialDisplay();

private:
    void construct();

    ShaderProgram* m_shader;

    DialogConvMode m_current_dialog_mode;

    Player* m_player;
    NPC* m_npc;
    DialogSceneNode* m_dialog_scene_node;
    Map* m_scene_one_map;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    glm::vec4 m_scene_one_map_boundary;

    std::unique_ptr<SceneNode> m_scene_one_root_node;
    SceneNode* m_scene_one_layer_node;
};
