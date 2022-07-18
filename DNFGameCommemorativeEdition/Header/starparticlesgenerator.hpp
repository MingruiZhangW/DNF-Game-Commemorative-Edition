#pragma once

#include "shaderprogram.hpp"
#include "texture.hpp"
#include "geometrynode.hpp"

/*
 * The star particles generator will generator a bunch of stars at the
 * celebration stage under the victory texture that are uniformly distributed
 * inside a circle.
 *
 * This will use Instancing:
 * https://learnopengl.com/Advanced-OpenGL/Instancing
 */
class StarParticlesGenerator : public GeometryNode
{
public:
    StarParticlesGenerator(ShaderProgram* shader);
    ~StarParticlesGenerator() {}

    void draw() override;
    void translate(const glm::vec3& amount) override;

    void updateParticleShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans);
    void updateParticleShaderModelMat(const glm::mat4& nodeTrans);

    void cleanMovement();

private:
    void processing();

    struct Particle
    {
        glm::vec3 positionOffset; // Where it is offset to be added in shader
        glm::vec3 direction;      // Where to go
        float speed;              // How fast
        float life;               // Remaining life of the particle.
    };

    // Global shader to disable
    ShaderProgram* m_shader;

    // Particle shader
    ShaderProgram m_particle_shader;
    GLint m_particle_p_uni;
    GLint m_particle_v_uni;
    GLint m_particle_m_uni;

    // Star texture
    Texture m_texture;

    // Particles Generator Vertex Array Object
    GLuint m_particles_generator_vao;
    // Particles Generator Basic Mesh Vertex Buffer Object
    GLuint m_particles_generator_basic_mesh_vbo;
    // Particles Generator Position Offset Vertex Buffer Object
    GLuint m_particles_generator_pos_offset_vbo;

    GLuint m_position_attrib_pos;
    GLuint m_pos_offset_attrib_pos;
    GLuint m_texture_coor_attrib_pos;

    // Move
    float m_dx;
    float m_dy;

    std::vector<Particle> m_particles;
};
