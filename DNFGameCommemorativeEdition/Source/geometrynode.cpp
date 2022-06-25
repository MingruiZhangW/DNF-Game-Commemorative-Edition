#include "geometrynode.hpp"

GeometryNode::GeometryNode(const std::string& name)
    : SceneNode(name)
{
    m_node_type = NodeType::GeometryNode;
}
