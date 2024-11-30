#pragma once
#include <string>

namespace cliService
{

  class NodeIf
  {
  public:
    explicit NodeIf(std::string name);
    virtual ~NodeIf() = default;
    
    const std::string& getName() const;
    NodeIf* getParent() const;
    void setParent(NodeIf* parent);
    
    bool isCommand() const;
    virtual bool isDirectory() const = 0;

  protected:
    std::string _name;
    NodeIf* _parent;
  };
  
}
