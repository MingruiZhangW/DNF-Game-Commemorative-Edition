#include "starparticlesgenerator.hpp"
#include "gamewindow.hpp"
#include "game.hpp"
#include "constant.hpp"

#include "glerrorcheck.hpp"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <random>
#include <cmath>

#define M_PI 3.14159265358979323846 /* pi */

// clang-format off

// Particles Generator vertex at the live area origin with texture coord
static const GLfloat particles_generator_vertex_buffer_data[] =
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

static const unsigned int max_particles_num {100};
static const unsigned int new_particles_per_time {16};
static const float radius_stars_circles {300.0f};
static float m_game_delta_time {0.0f};
static float m_star_size {30.0f};
static float m_star_life {1.0f};

// Use for glVertexAttribDivisor and glDrawArraysInstanced
// 3 because of x,y,z
static GLfloat* particle_pos_offset_data = new GLfloat[max_particles_num * 3];

StarParticlesGenerator::StarParticlesGenerator(ShaderProgram* shader)
    : GeometryNode(StringContant::starParticlesGeneratorName)
    , m_shader(shader)
{
    // Create particle shader
    m_particle_shader.generateProgramObject();
    m_particle_shader.attachVertexShader(ParticleShaderPath::vertexShader.c_str());
    m_particle_shader.attachFragmentShader(ParticleShaderPath::fragmentShader.c_str());
    m_particle_shader.link();

    // Set up the uniforms
    m_particle_p_uni = m_particle_shader.getUniformLocation("P");
    m_particle_v_uni = m_particle_shader.getUniformLocation("V");
    m_particle_m_uni = m_particle_shader.getUniformLocation("M");

    // Load texture
    m_texture = Texture(TexturePath::starPNGPath);
    m_texture.loadTexture();

    // Reserve memory - size unchanged
    m_particles.reserve(max_particles_num);

    // Use current time as seed for random generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Init m_trans
    m_trans = glm::scale(m_trans, glm::vec3(m_star_size, m_star_size, m_star_size));
    m_trans = glm::translate(m_trans, glm::vec3(0.5f, 0.5f, 0.0f));
    m_trans = glm::rotate(m_trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_trans = glm::translate(m_trans, glm::vec3(-0.5f, -0.5f, 0.0f));

    // Create the vertex array to record buffer assignments for particles generator.
    glGenVertexArrays(1, &m_particles_generator_vao);
    glBindVertexArray(m_particles_generator_vao);

    // Create the particles generator vertex buffer
    glGenBuffers(1, &m_particles_generator_basic_mesh_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_particles_generator_basic_mesh_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(particles_generator_vertex_buffer_data),
                 particles_generator_vertex_buffer_data,
                 GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    m_position_attrib_pos = m_particle_shader.getAttribLocation("position");
    glEnableVertexAttribArray(m_position_attrib_pos);
    glVertexAttribPointer(m_position_attrib_pos,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(particles_generator_vertex_buffer_data[0]) * 5,
                          nullptr);

    // texture coord
    m_texture_coor_attrib_pos = m_particle_shader.getAttribLocation("textureCoor");
    glEnableVertexAttribArray(m_texture_coor_attrib_pos);
    glVertexAttribPointer(m_texture_coor_attrib_pos,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(particles_generator_vertex_buffer_data[0]) * 5,
                          (void*) (sizeof(particles_generator_vertex_buffer_data[0]) * 3));

    /*
     * By default, the attribute divisor is 0 which tells OpenGL to update the content of the vertex
     * attribute each iteration of the vertex shader. (Normal behavior new vertex come in)
     *
     * By setting this attribute to 1 we're telling OpenGL that we want to
     * update the content of the vertex attribute when we start to render a new instance.
     *
     * By setting it to 2 we'd update the content every 2 instances and so on.
     */
    glVertexAttribDivisor(m_position_attrib_pos, 0);
    glVertexAttribDivisor(m_texture_coor_attrib_pos, 0);

    // pos offset
    m_pos_offset_attrib_pos = m_particle_shader.getAttribLocation("posOffset");
    glEnableVertexAttribArray(m_pos_offset_attrib_pos);
    // Note that, glBindBuffer should be before glVertexAttribPointer !!!!!!!!!!
    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &m_particles_generator_pos_offset_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_particles_generator_pos_offset_vbo);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, max_particles_num * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_pos_offset_attrib_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glVertexAttribDivisor(m_pos_offset_attrib_pos, 1);

    // Reset state to prevent rogue code from messing with *my* stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void
StarParticlesGenerator::processing()
{
    /* Generate.
     * How to generate a random point within a circle of radius R.
     * https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
     *
     * We generate stars randomly in a circle and get its direction,
     * The position offset will then be updated constantly and will be added
     * in the shader.
     *
     */

    if (m_particles.size() < max_particles_num) {
        auto rest = max_particles_num - m_particles.size();
        auto numToGen = (rest > new_particles_per_time) ? new_particles_per_time : rest;
        for (int i = 0; i < numToGen; i++) {
            Particle newPar {};

            // This particle will live m_star_life seconds.
            newPar.life = m_star_life;
            newPar.positionOffset = glm::vec3(0.0f, 0.0f, 0.0f);
            newPar.speed = 20.0f;

            auto r = radius_stars_circles * sqrt(Utils::randomBetweenZeroAndOne());
            auto theta = Utils::randomBetweenZeroAndOne() * 2.0f * M_PI;
            auto x = r * cos(theta);
            auto y = r * sin(theta);

            newPar.direction = glm::vec3(x, y, 0.0f);

            m_particles.push_back(newPar);
        }
    }

    // Simulate.
    auto deltaTime = GameWindow::getDeltaTime();
    for (int i = 0; i < m_particles.size(); i++) {
        if (m_particles[i].life > 0.0f) {
            // Decrease life
            m_particles[i].life -= deltaTime;
            if (m_particles[i].life > 0.0f) {
                // Move particles
                m_particles[i].positionOffset += m_particles[i].direction / m_particles[i].speed;

                // Fill buffer
                particle_pos_offset_data[3 * i + 0] = m_particles[i].positionOffset.x;
                particle_pos_offset_data[3 * i + 1] = m_particles[i].positionOffset.y;
                particle_pos_offset_data[3 * i + 2] = m_particles[i].positionOffset.z;
            } else {
                // Remove dead particles
                m_particles.erase(m_particles.begin() + i);
            }
        }
    }

    // Update pos size
    glEnableVertexAttribArray(m_pos_offset_attrib_pos);
    glBindBuffer(GL_ARRAY_BUFFER, m_particles_generator_pos_offset_vbo);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    max_particles_num * sizeof(GLfloat) * 3,
                    particle_pos_offset_data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(m_pos_offset_attrib_pos);

    CHECK_GL_ERRORS;
}

void
StarParticlesGenerator::translate(const glm::vec3& amount)
{
    m_dx = m_dx + amount.x;
    m_dy = m_dy + amount.y;
    SceneNode::translate(amount);
}

void
StarParticlesGenerator::draw()
{
    m_shader->disable();
    m_particle_shader.enable();

    processing();

    // Draw transparent backgrounds in blend mode (alpha channel)
    // https://stackoverflow.com/questions/3388294/opengl-question-about-the-usage-of-gldepthmask
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_texture.useTexture();

    // Draw the particules
    glBindVertexArray(m_particles_generator_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(m_particles.size()));
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    m_particle_shader.disable();
    m_shader->enable();
}

void
StarParticlesGenerator::updateParticleShaderModelMat(const glm::mat4& nodeTrans)
{
    m_shader->disable();
    m_particle_shader.enable();

    //-- Set ModelView matrix:
    glUniformMatrix4fv(m_particle_m_uni, 1, GL_FALSE, value_ptr(nodeTrans));

    m_particle_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS;
}

void
StarParticlesGenerator::updateParticleShaderPVMat(const glm::mat4& pTrans, const glm::mat4& vTrans)
{
    m_shader->disable();
    m_particle_shader.enable();

    //-- Set PV matrix:
    glUniformMatrix4fv(m_particle_p_uni, 1, GL_FALSE, value_ptr(pTrans));
    glUniformMatrix4fv(m_particle_v_uni, 1, GL_FALSE, value_ptr(vTrans));

    m_particle_shader.disable();
    m_shader->enable();

    CHECK_GL_ERRORS
}