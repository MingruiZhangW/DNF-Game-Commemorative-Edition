#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"

#include "GL/glew.h"

#include <memory>

class Map;
class Player;
class Monster;
class DialogSceneNode;

class SceneTwo
{
public:
    // TODO: add finite state machine.
    enum class DialogConvMode { ConvOne, ConvTwo };

    SceneTwo(ShaderProgram* shader,
             GLfloat frameBufferWidth,
             GLfloat frameBufferHeight,
             Player* player,
             DialogSceneNode* dialogSceneNode);

    ~SceneTwo();

    SceneNode* getRootSceneNode()
    {
        return m_scene_two_root_node.get();
    }

    glm::vec4 getSceneTwoMapBoundary()
    {
        return m_scene_two_map_boundary;
    }

    std::vector<Monster*>& getMonsters()
    {
        return m_monsters;
    }

    std::pair<bool, bool> sceneTwoCollisionTest(const glm::vec2& movement);
    std::pair<Monster*, bool> sceneTwoAttackCollisionTest();

    // Layered drawing
    void reorderLayerNodeChild();

    // Dialog
    void moveDialog(float dx);
    bool processClick();

    // Initial display, should call first.
    void prepareInitialDisplay();

    // Remove monster what are killed
    void checkToRemoveMonster();

private:
    void construct();

    ShaderProgram* m_shader;

    DialogConvMode m_current_dialog_mode;

    Player* m_player;
    DialogSceneNode* m_dialog_scene_node;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    glm::vec4 m_scene_two_map_boundary;

    std::unique_ptr<SceneNode> m_scene_two_root_node;
    SceneNode* m_scene_two_layer_node;

    int m_monster_nums;
    std::vector<Monster*> m_monsters;

    Map* m_scene_two_map;
};
