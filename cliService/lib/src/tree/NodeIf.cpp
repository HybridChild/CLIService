#include "cliService/tree/NodeIf.hpp"

namespace cliService
{

  NodeIf::NodeIf(std::string name)
    : _name(std::move(name))
    , _parent(nullptr)
  {}

  const std::string& NodeIf::getName() const { return _name; }
  NodeIf* NodeIf::getParent() const { return _parent; }
  void NodeIf::setParent(NodeIf* parent) { _parent = parent; }
  bool NodeIf::isCommand() const { return !isDirectory(); }

}
