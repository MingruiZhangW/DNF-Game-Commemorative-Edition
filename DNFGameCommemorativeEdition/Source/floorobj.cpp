#include "floorobj.hpp"
#include "gamewindow.hpp"
#include "constant.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>

// clang-format off

// Floor Obj vertex at the live area origin with texture coord
static const GLfloat floor_obj_vertex_buffer_data[] =
{
    // triangle 1
    // x,    y,    z,    u,    v,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // triangle 2
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f
};

// clang-format on

FloorObj::FloorObj(const std::string& name, ShaderProgram* shader, FloorObjType textureType)
    : GeometryNode(name)
    , m_shader(shader)
    , m_floor_obj_type(textureType)
    , m_floor_obj_collide_height(30.0f)
{
    // Load texture
    switch (m_floor_obj_type) {
    case FloorObjType::BossDoor:
        m_texture = Texture(TexturePath::doorPath);
        m_door_effect_texture = Texture(TexturePath::doorBossEffectPath);

        m_texture.loadTexture();
        m_door_effect_texture.loadTexture();
        break;
    case FloorObjType::NormalDoor:
        m_texture = Texture(TexturePath::doorPath);
        m_door_effect_texture = Texture(TexturePath::doorNormalEffectPath);

        m_texture.loadTexture();
        m_door_effect_texture.loadTexture();
        break;
    case FloorObjType::DoorTree:
        m_texture = Texture(TexturePath::doorTreePath);

        m_texture.loadTexture();
        break;
    case FloorObjType::Tree:
        m_texture = Texture(TexturePath::treePath);

        m_texture.loadTexture();
        break;
    default:
        break;
    }

    m_plane_width = static_cast<float>(m_texture.getTextureWidth());
    m_plane_height = static_cast<float>(m_texture.getTextureHeight());

    // S * T * R * T^-1
    // Scale will have no effect on translation
    m_trans = glm::scale(m_trans, glm::vec3(m_plane_width, m_plane_height, 0.0f));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for floor.
    glGenVertexArrays(1, &m_floor_obj_vao);
    glBindVertexArray(m_floor_obj_vao);

    // Create the floor vertex buffer
    glGenBuffers(1, &m_floor_obj_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_floor_obj_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(floor_obj_vertex_buffer_data),
                 floor_obj_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_shader->getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(floor_obj_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_shader->getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(floor_obj_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(floor_obj_vertex_buffer_data[0]) * 3));

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
FloorObj::draw()
{
    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_texture.useTexture();

    glBindVertexArray(m_floor_obj_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
    glBindVertexArray(0);

    switch (m_floor_obj_type) {
    case FloorObjType::NormalDoor:
    case FloorObjType::BossDoor:
        m_door_effect_texture.useTexture();

        glBindVertexArray(m_floor_obj_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3 * 2);
        glBindVertexArray(0);
        break;
    default:
        break;
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
