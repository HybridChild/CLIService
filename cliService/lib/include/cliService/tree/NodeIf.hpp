#pragma once
#include <string>

namespace cliService
{
  
  enum class AccessLevel;

  class NodeIf
  {
  public:
    explicit NodeIf(std::string name, AccessLevel level);
    virtual ~NodeIf() = default;
    
    virtual bool isDirectory() const = 0;
    bool isCommand() const;

    const std::string& getName() const;
    NodeIf* getParent() const;
    void setParent(NodeIf* parent);
    AccessLevel getAccessLevel() const;

  protected:
    std::string _name;
    NodeIf* _parent;
    AccessLevel _accessLevel;
  };

}
