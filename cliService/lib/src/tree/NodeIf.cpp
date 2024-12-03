#include "cliService/tree/NodeIf.hpp"

namespace cliService
{

  NodeIf::NodeIf(std::string name, AccessLevel level)
    : _name(std::move(name))
    , _parent(nullptr)
    , _accessLevel(level)
  {}

  bool NodeIf::isCommand() const { return !isDirectory(); }

  const std::string& NodeIf::getName() const { return _name; }

  NodeIf* NodeIf::getParent() const { return _parent; }

  void NodeIf::setParent(NodeIf* parent) { _parent = parent; }

  AccessLevel NodeIf::getAccessLevel() const { return _accessLevel; }

}
