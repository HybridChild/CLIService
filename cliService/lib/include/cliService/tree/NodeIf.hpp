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
    bool isCommand() const { return !isDirectory(); }

    const std::string& getName() const { return _name; };
    NodeIf* getParent() const { return _parent; }
    void setParent(NodeIf* parent) { _parent = parent; }
    AccessLevel getAccessLevel() const { return _accessLevel; }

  protected:
    std::string _name;
    NodeIf* _parent;
    AccessLevel _accessLevel;
  };

}
