#include "cliService/tree/Node.hpp"

namespace cliService
{

  Node::Node(std::string name)
    : _name(std::move(name))
    , _parent(nullptr)
  {}

  const std::string& Node::getName() const { return _name; }
  Node* Node::getParent() const { return _parent; }
  void Node::setParent(Node* parent) { _parent = parent; }
  bool Node::isCommand() const { return !isDirectory(); }

}
