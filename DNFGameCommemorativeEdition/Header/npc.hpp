#pragma once

#include "geometrynode.hpp"
#include "shaderprogram.hpp"
#include "texture.hpp"
#include "constant.hpp"

#include "json.hpp"

#include <glm.hpp>

#include <vector>

/*
 * We are only going to have one NPC here, no extendibility is considered here.
 */

using json = nlohmann::json;

class NPC : public GeometryNode
{
public:
    NPC(ShaderProgram* shader);
    ~NPC() {}

    // Implement virtual function from base class
    void draw();
    void translate(const glm::vec3& amount);

    // x, y -> x, y, x,y at bottom - left corner
    // z -> width
    // w -> height
    glm::vec4 getNPCCollideGeo();

    glm::vec2 getNPCCenter()
    {
        return m_npc_center;
    }

    float getNPCDx()
    {
        return m_npc_dx;
    }

    float getNPCDy()
    {
        return m_npc_dy;
    }

    // Update shadow shader matrix uniforms
    void updateShadowShaderModelMat(const glm::mat4& nodeTrans);
    void updateShadowShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans);

private:
    void updateTexCoord();
    void updateFrame();

    ShaderProgram* m_shader;

    // Uniform location for PVM matrix for shadow shader.
    // Shadow is created by shear transformation and assign black/alpha value
    // in fragement shader
    GLint m_shadow_p_uni;
    GLint m_shadow_v_uni;
    GLint m_shadow_m_uni;
    ShaderProgram m_shadow_shader;

    GLuint m_position_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // NPC Vertex Array Object
    GLuint m_npc_vao;
    // NPC Vertex Buffer Object
    GLuint m_npc_vbo;
    // NPC Texture Vertex Buffer Object
    GLuint m_npc_tex_vbo;

    GLfloat m_npc_width;
    GLfloat m_npc_height;

    glm::vec2 m_npc_center;
    GLfloat m_npc_dx;
    GLfloat m_npc_dy;

    // Animation
    // Animation speed here is divided by 1000
    json m_npc_animation_json_parser;
    unsigned int m_number_of_frames;
    float m_animation_speed;
    std::string m_current_frame;
    Texture m_textures_sheet;

    float m_animation_cursor;
};
