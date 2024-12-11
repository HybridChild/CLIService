#pragma once
#include "cliService/tree/NodeIf.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Path.hpp"
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
    T& addCommand(std::string name, AccessLevel level, std::string description = "")
    {
      auto cmd = std::make_unique<T>(name, level, description);
      T* cmdPtr = cmd.get();
      addChild(std::move(cmd));
      return *cmdPtr;
    }

    NodeIf* findNode(const std::vector<std::string>& path) const;
    void traverse(const std::function<void(const NodeIf&, int)>& visitor, int depth = 0) const;

    NodeIf* resolvePath(std::string_view pathStr, const Directory& currentDir) const;
    Path getRelativePath(const NodeIf& node) const;

  private:
    void addChild(std::unique_ptr<NodeIf> child);
    std::vector<std::unique_ptr<NodeIf>> _children;
  };

}
