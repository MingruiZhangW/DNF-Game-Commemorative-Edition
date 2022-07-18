#pragma once

#include "scenenode.hpp"
#include "shaderprogram.hpp"
#include "constant.hpp"

#include "GL/glew.h"

#include <memory>

class Map;
class Player;
class Button;
class Monster;
class DialogSceneNode;
class StarParticlesGenerator;

class SceneTwo
{
public:
    // TODO: add finite state machine.
    enum class SceneTwoStage { ConvOne, Fighting, ConvTwo, Victory, Defeat };

    SceneTwo(ShaderProgram* shader,
             GLfloat frameBufferWidth,
             GLfloat frameBufferHeight,
             Player* player,
             DialogSceneNode* dialogSceneNode,
             StarParticlesGenerator* starParticlesGenerator);

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
    std::vector<std::pair<Monster*, bool>> sceneTwoAttackCollisionTest();

    // Layered drawing
    void reorderLayerNodeChild();

    // Dialog and mouse processing
    void moveDialog(float dx);
    Scene::SceneEvents processClick();
    bool processHover(const glm::vec2& mousePos);

    // Initial display, should call first.
    void prepareInitialDisplay();

    // Remove monster what are killed
    void checkToRemoveMonster();

    // Flock monster movements
    void updateMonsterFlockingMovements();

private:
    void construct();

    ShaderProgram* m_shader;

    SceneTwoStage m_current_stage;

    Player* m_player;
    StarParticlesGenerator* m_star_particles_generator;
    DialogSceneNode* m_dialog_scene_node;
    Button* m_exit_button;
    Button* m_victory_logo;

    float m_scene_dx;

    bool m_hover_changed;

    int m_monster_nums;
    std::vector<Monster*> m_monsters;

    Map* m_scene_two_map;
    glm::vec4 m_scene_two_map_boundary;

    GLfloat m_frame_buffer_width;
    GLfloat m_frame_buffer_height;

    std::unique_ptr<SceneNode> m_scene_two_root_node;
    SceneNode* m_scene_two_layer_node;
};
