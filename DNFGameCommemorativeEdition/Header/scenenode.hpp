#pragma once

#include <glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include <functional>

enum class NodeType { SceneNode, GeometryNode };

class SceneNode
{
public:
    SceneNode(const std::string& name);

    SceneNode(const SceneNode& other);

    virtual ~SceneNode();

    virtual void draw() {};

    const glm::mat4& getTransform() const;
    const glm::mat4& getInverse() const;

    void setTransform(const glm::mat4& m);

    void addChild(SceneNode* child);

    void removeChild(SceneNode* child);

    //-- Transformations:
    virtual void rotate(char axis, float angle);
    virtual void scale(const glm::vec3& amount);
    virtual void translate(const glm::vec3& amount);

    static void nAryChildTraversal(SceneNode* sceneNode,
                                   std::function<void(SceneNode*)> callbackFunc);

    friend std::ostream& operator<<(std::ostream& os, const SceneNode& node);

    // Transformations
    glm::mat4 m_trans;
    glm::mat4 m_invtrans;

    std::list<SceneNode*> m_children;

    NodeType m_node_type;
    std::string m_name;
};
