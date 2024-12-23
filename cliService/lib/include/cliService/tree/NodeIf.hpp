#pragma once
#include <string>

namespace cliService
{

  enum class AccessLevel;

  class NodeIf
  {
  public:
    explicit NodeIf(std::string name, AccessLevel level)
      : _name(std::move(name))
      , _parent(nullptr)
      , _accessLevel(level)
    {}

    virtual ~NodeIf() = default;
    
    virtual bool isDirectory() const = 0;

    const std::string& getName() const { return _name; };
    AccessLevel getAccessLevel() const { return _accessLevel; }

    NodeIf* getParent() const { return _parent; }
    void setParent(NodeIf* parent) { _parent = parent; }

  protected:
    std::string _name;
    NodeIf* _parent;
    AccessLevel _accessLevel;
  };

}
