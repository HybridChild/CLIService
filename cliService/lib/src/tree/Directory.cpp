#include "cliService/tree/Directory.hpp"
#include <algorithm>

namespace cliService
{

  Directory::Directory(std::string name)
    : Node(std::move(name))
  {}

  bool Directory::isDirectory() const
  {
    return true;
  }

  Directory& Directory::addDirectory(const std::string& name)
  {
    auto dir = std::make_unique<Directory>(name);
    Directory* dirPtr = dir.get();
    addChild(std::move(dir));
    return *dirPtr;
  }

  Node* Directory::findNode(const std::vector<std::string>& path)
  {
    if (path.empty()) {
      return this;
    }

    auto it = std::find_if(_children.begin(), _children.end(),
      [&path](const auto& child) {
        return child->getName() == path[0];
      });

    if (it == _children.end()) {
      return nullptr;
    }

    if (path.size() == 1) {
      return it->get();
    }

    if (!(*it)->isDirectory()) {
      return nullptr;
    }

    std::vector<std::string> subPath(path.begin() + 1, path.end());
    return static_cast<Directory*>(it->get())->findNode(subPath);
  }

  void Directory::traverse(const std::function<void(const Node&, int)>& visitor, int depth) const
  {
    visitor(*this, depth);
    for (const auto& child : _children)
    {
      if (child->isDirectory()) {
        static_cast<const Directory*>(child.get())->traverse(visitor, depth + 1);
      }
      else {
        visitor(*child, depth + 1);
      }
    }
  }

  void Directory::addChild(std::unique_ptr<Node> child)
  {
    auto it = std::find_if(_children.begin(), _children.end(),
      [&child](const auto& existing) {
        return existing->getName() == child->getName();
      });

    assert(it == _children.end() && "Name collision in directory");

    child->setParent(this);
    _children.push_back(std::move(child));
  }

}
