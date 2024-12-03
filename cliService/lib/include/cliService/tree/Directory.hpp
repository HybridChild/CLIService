#pragma once
#include "NodeIf.hpp"
#include "CommandIf.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <cassert>

namespace cliService
{

  class Directory : public NodeIf
  {
  public:
    explicit Directory(std::string name, AccessLevel level);
    bool isDirectory() const override;

    Directory& addDirectory(const std::string& name, AccessLevel level);
    
    template<typename T>
    T& addCommand()
    {
      auto cmd = std::make_unique<T>();
      T* cmdPtr = cmd.get();
      addChild(std::move(cmd));
      return *cmdPtr;
    }

    NodeIf* findNode(const std::vector<std::string>& path);
    void traverse(const std::function<void(const NodeIf&, int)>& visitor, int depth = 0) const;

  private:
    void addChild(std::unique_ptr<NodeIf> child);
    std::vector<std::unique_ptr<NodeIf>> _children;
  };

}
