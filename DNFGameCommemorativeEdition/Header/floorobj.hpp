#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

class FloorObj : public GeometryNode
{
public:
    enum class FloorObjType { Tree, DoorBush, FlowerOne, FlowerTwo, SideNormalDoor };

    FloorObj(const std::string& name, ShaderProgram* shader, FloorObjType textureType);
    ~FloorObj() {}

    void draw() override;
    glm::vec2 getTextureGeo()
    {
        return glm::vec2(m_texture.getTextureWidth(), m_texture.getTextureHeight());
    }

    glm::vec2 getCollisionWH()
    {
        return glm::vec2(getTextureGeo().x, m_floor_obj_collide_height);
    }

    FloorObjType getFloorObjType()
    {
        return m_floor_obj_type;
    }

    void setCollisionHeight(float collisionHeight)
    {
        m_floor_obj_collide_height = collisionHeight;
    }

    void flip();

private:
    ShaderProgram* m_shader;

    GLfloat m_plane_width;
    GLfloat m_plane_height;

    Texture m_texture;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    FloorObjType m_floor_obj_type;

    // Floor Obj Vertex Array Object
    GLuint m_floor_obj_vao;
    // Floor Obj Vertex Buffer Object
    GLuint m_floor_obj_vbo;

    float m_floor_obj_collide_height;
};
