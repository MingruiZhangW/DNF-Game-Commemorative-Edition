#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

class FloorObj : public GeometryNode
{
public:
    enum class FloorObjType { Tree, DoorTree, NormalDoor, BossDoor };

    FloorObj(const std::string& name, ShaderProgram* shader, FloorObjType textureType);
    ~FloorObj() {}

    void draw();
    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

    glm::vec2 getCollisionWH()
    {
        switch (m_floor_obj_type) {
        case FloorObjType::Tree:
            return glm::vec2(getTextureGeo().x, m_floor_obj_collide_height);
        case FloorObj::FloorObjType::DoorTree:
        case FloorObj::FloorObjType::NormalDoor:
        case FloorObj::FloorObjType::BossDoor:
            return getTextureGeo();
        default:
            break;
        }

        return glm::vec2(0.0f);
    }

private:
    ShaderProgram* m_shader;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    Texture m_texture;
    Texture m_door_effect_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    FloorObjType m_floor_obj_type;

    // Floor Obj Vertex Array Object
    GLuint m_floor_obj_vao;
    // Floor Obj Vertex Buffer Object
    GLuint m_floor_obj_vbo;

    float m_floor_obj_collide_height;
};
