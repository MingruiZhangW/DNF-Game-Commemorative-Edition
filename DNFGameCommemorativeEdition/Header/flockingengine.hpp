#pragma once

#include <glm.hpp>

#include <vector>

/*
 * Flocking algorithm is stateless, so not much to store here.
 *
 * We will implement:
 *      Cohesion:   Have each unit steer toward the average position of its neighbors.
 *      Alignment:  Have each unit steer so as to align itself to the average heading
                    of its neighbors.
 *      Separation: Have each unit steer to avoid hitting its neighbors.
 *
 */

class Monster;

class FlockingEngine
{
public:
    FlockingEngine();
    ~FlockingEngine();

    static void updatePosition(std::vector<Monster*>& monsters);
};