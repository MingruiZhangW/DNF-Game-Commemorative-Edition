#include "scenenode.hpp"

#include <iostream>
#include <sstream>

#include <glm.hpp>
#include <gtx/transform.hpp>

SceneNode::SceneNode(const std::string& name)
    : m_name(name)
    , m_node_type(NodeType::SceneNode)
    , m_trans(glm::mat4(1.0f))
{}

// Deep copy
SceneNode::SceneNode(const SceneNode& other)
    : m_node_type(other.m_node_type)
    , m_name(other.m_name)
    , m_trans(other.m_trans)
    , m_invtrans(other.m_invtrans)
{
    for (SceneNode* child : other.m_children) {
        this->m_children.push_front(new SceneNode(*child));
    }
}

SceneNode::~SceneNode()
{
    for (SceneNode* child : m_children) {
        delete child;
    }
}

void
SceneNode::setTransform(const glm::mat4& m)
{
    m_trans = m;
    m_invtrans = m;
}

const glm::mat4&
SceneNode::getTransform()
{
    return m_trans;
}

const glm::mat4&
SceneNode::getInverse()
{
    return m_invtrans;
}

void
SceneNode::addChild(SceneNode* child)
{
    m_children.push_back(child);
}

void
SceneNode::addChildFront(SceneNode* child)
{
    m_children.insert(m_children.begin(), child);
}

void
SceneNode::removeChild(SceneNode* child)
{
    m_children.remove(child);
}

void
SceneNode::cleanChild()
{
    m_children.clear();
}

void
SceneNode::rotate(char axis, float angle)
{
    glm::vec3 rot_axis;

    switch (axis) {
    case 'x':
        rot_axis = glm::vec3(1, 0, 0);
        break;
    case 'y':
        rot_axis = glm::vec3(0, 1, 0);
        break;
    case 'z':
        rot_axis = glm::vec3(0, 0, 1);
        break;
    default:
        break;
    }
    glm::mat4 rot_matrix = glm::rotate(glm::radians(angle), rot_axis);
    m_trans = rot_matrix * m_trans;
}

void
SceneNode::scale(const glm::vec3& amount)
{
    m_trans = glm::scale(amount) * m_trans;
}

void
SceneNode::translate(const glm::vec3& amount)
{
    m_trans = glm::translate(amount) * m_trans;
}

void
SceneNode::nAryChildTraversal(SceneNode* sceneNode, std::function<void(SceneNode*)> callbackFunc)
{
    if (sceneNode == nullptr)
        return;

    // Call callbackFunc here
    callbackFunc(sceneNode);

    // All the children except the last
    for (auto i : sceneNode->m_children) {
        nAryChildTraversal(i, callbackFunc);
    }
}

std::ostream&
operator<<(std::ostream& os, const SceneNode& node)
{
    // os << "SceneNode:[NodeType: ___, name: ____, id: ____, transform: ____"
    switch (node.m_node_type) {
    case NodeType::SceneNode:
        os << "SceneNode";
        break;
    case NodeType::GeometryNode:
        os << "GeometryNode";
        break;
    }
    os << ":[";
    os << "name:" << node.m_name << ", ";
    os << "]";

    return os;
}
