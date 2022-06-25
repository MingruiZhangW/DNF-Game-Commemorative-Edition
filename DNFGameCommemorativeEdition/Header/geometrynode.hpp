#pragma once

#include "scenenode.hpp"

class GeometryNode : public SceneNode
{
public:
    GeometryNode(const std::string& name);

    virtual void draw() {}
};
