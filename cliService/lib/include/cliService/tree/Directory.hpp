#pragma once
#include "cliService/tree/NodeIf.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Path.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <type_traits>
#include <cassert>
#include <variant>

namespace cliService
{

  class Directory : public NodeIf
  {
  public:
    explicit Directory(std::string name, AccessLevel level);

    bool isDirectory() const override { return true; }

    NodeIf* findNode(const std::vector<std::string>& path) const;
    NodeIf* resolvePath(std::string_view pathStr, const Directory& currentDir) const;
    Path getRelativePath(const NodeIf& node) const;
    void traverse(const std::function<void(const NodeIf&, size_t)>& visitor, size_t depth = 0) const;

    // Add references to statically allocated nodes
    void addStatic(Directory& dir)
    {
      checkNameCollision(dir.getName());
      dir.setParent(this);
      _children.emplace_back(&dir);
    }

    void addStatic(CommandIf& cmd)
    {
      checkNameCollision(cmd.getName());
      cmd.setParent(this);
      _children.emplace_back(&cmd);
    }

    // Create and add dynamically allocated nodes
    Directory& addDynamicDirectory(const std::string& name, AccessLevel level)
    {
      checkNameCollision(name);
      auto dir = std::make_unique<Directory>(name, level);
      Directory* dirPtr = dir.get();
      dirPtr->setParent(this);
      _children.emplace_back(std::move(dir));
      return *dirPtr;
    }
    
    template<typename T>
    T& addDynamicCommand(std::string name, AccessLevel level, std::string description = "")
    {
      static_assert(std::is_base_of_v<CommandIf, T>, "T must derive from CommandIf");
      
      checkNameCollision(name);
      auto cmd = std::make_unique<T>(std::move(name), level, std::move(description));
      T* cmdPtr = cmd.get();
      cmdPtr->setParent(this);
      _children.emplace_back(std::move(cmd));
      return *cmdPtr;
    }

  private:
    using ChildPtr = std::variant<NodeIf*, std::unique_ptr<NodeIf>>;
    std::vector<ChildPtr> _children;

    NodeIf* getNodePtr(const ChildPtr& child) const
    {
      if (auto staticPtr = std::get_if<NodeIf*>(&child)) {
        return *staticPtr;
      }
      
      return std::get<std::unique_ptr<NodeIf>>(child).get();
    }

    void checkNameCollision(const std::string& name) const;
  };

}