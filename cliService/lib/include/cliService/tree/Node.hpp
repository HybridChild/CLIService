#pragma once
#include <string>

namespace cliService
{

  class Node
  {
  public:
    explicit Node(std::string name);
    virtual ~Node() = default;
    
    const std::string& getName() const;
    Node* getParent() const;
    void setParent(Node* parent);
    
    bool isCommand() const;
    virtual bool isDirectory() const = 0;

  protected:
    std::string _name;
    Node* _parent;
  };
  
}
