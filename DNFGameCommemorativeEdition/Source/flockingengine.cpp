#include "flockingengine.hpp"
#include "monster.hpp"
#include "gamewindow.hpp"

// Const
static const float m_alignment_speed {1.0f};
static const float m_cohesion_speed {0.01f};
static const float m_monster_separation_speed {0.01f};
static const float m_boundary_separation_speed {5.0f};
static const float m_min_distance_x {200.0f};
static const float m_min_distance_y {100.0f};
static const float m_update_time {2.0f};

static glm::vec3 m_random_trans {Utils::randomBetween(-2, 2), Utils::randomBetween(-2, 2), 0.0f};
static float m_accumulated_time {0.0f};

glm::vec3
getRandomTrans()
{
    m_accumulated_time += GameWindow::getDeltaTime();
    if (m_accumulated_time > m_update_time) {
        m_random_trans = glm::vec3(Utils::randomBetween(-2, 2), Utils::randomBetween(-2, 2), 0.0f);
        m_accumulated_time = 0.0f;
    }

    return m_random_trans;
}

FlockingEngine::FlockingEngine() {}

FlockingEngine ::~FlockingEngine() {}

void
FlockingEngine::updatePosition(std::vector<Monster*>& monsters)
{
    std::vector<glm::vec3> transMap;
    glm::vec3 translation {getRandomTrans()};
    glm::vec3 lastTranslationTogether {0.0f};
    glm::vec3 posTogerther {0.0f};

    for (int i = 0; i < monsters.size(); i++) {
        // Total neighbor for monster i
        float totalNeighbor = 0;
        for (int j = 0; j < monsters.size(); j++) {
            // i -> monster in use
            // j -> monster to compare

            if (i == j)
                continue;

            // Separation between monsters
            auto plainXDiff = monsters[i]->getMonsterDx() - monsters[j]->getMonsterDx();
            auto plainYDiff = monsters[i]->getMonsterDy() - monsters[j]->getMonsterDy();
            if (abs(plainXDiff) <= m_min_distance_x || abs(plainYDiff) <= m_min_distance_y) {
                // Move a little bit in an another direction
                translation.x += abs(plainXDiff) <= m_min_distance_x
                                     ? m_monster_separation_speed * -glm::sign(plainXDiff)
                                     : 0.0f;
                translation.y += abs(plainYDiff) <= m_min_distance_y
                                     ? m_monster_separation_speed * -glm::sign(plainYDiff)
                                     : 0.0f;

                // Collect data
                lastTranslationTogether += monsters[j]->getLastMonsterTrans();
                posTogerther.x += monsters[j]->getMonsterDx();
                posTogerther.y += monsters[j]->getMonsterDy();
                totalNeighbor++;
            }
        }

        if (totalNeighbor > 0) {
            // Cohesion - need to calculate a direction since average pos is a point
            posTogerther = posTogerther / totalNeighbor;
            glm::vec3 directionToCenterOfPos = posTogerther
                                               - glm::vec3(monsters[i]->getMonsterDx(),
                                                           monsters[i]->getMonsterDy(),
                                                           0.0f);
            translation = translation + glm::normalize(directionToCenterOfPos) * m_cohesion_speed;

            // Alignment - is a direction itself
            lastTranslationTogether = glm::normalize(lastTranslationTogether / totalNeighbor);
            translation = translation + m_alignment_speed * lastTranslationTogether;
        }

        // Separation between map boundary
        auto hitBoundary = monsters[i]->checkHitMapBoundary(translation);
        if (hitBoundary.first.first) {
            translation.x += hitBoundary.first.second ? m_boundary_separation_speed
                                                      : -m_boundary_separation_speed;
        }
        if (hitBoundary.second.first) {
            translation.y += hitBoundary.second.second ? m_boundary_separation_speed
                                                       : -m_boundary_separation_speed;
        }

        transMap.push_back(translation);
    }

    for (int k = 0; k < monsters.size(); k++) {
        if (!monsters[k]->lockForMovement())
            monsters[k]->moveMonster(transMap[k]);
    }
}