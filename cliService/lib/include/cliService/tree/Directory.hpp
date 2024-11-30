#pragma once
#include "Node.hpp"
#include "Command.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <cassert>

namespace cliService
{

  class Directory : public Node
  {
  public:
    explicit Directory(std::string name);
    bool isDirectory() const override;

    Directory& addDirectory(const std::string& name);
    
    template<typename T>
    T& addCommand()
    {
      auto cmd = std::make_unique<T>();
      T* cmdPtr = cmd.get();
      addChild(std::move(cmd));
      return *cmdPtr;
    }

    Node* findNode(const std::vector<std::string>& path);
    void traverse(const std::function<void(const Node&, int)>& visitor, int depth = 0) const;

  private:
    void addChild(std::unique_ptr<Node> child);
    std::vector<std::unique_ptr<Node>> _children;
  };

}
