#include "cliService/tree/Directory.hpp"
#include "cliService/tree/PathResolver.hpp"
#include <algorithm>

namespace cliService
{

  Directory::Directory(std::string name, AccessLevel level)
    : NodeIf(std::move(name), level)
  {}


  NodeIf* Directory::findNode(const std::vector<std::string>& path) const
  {
    if (path.empty()) { return const_cast<Directory*>(this); }

    auto it = std::find_if(_children.begin(), _children.end(),
      [&path, this](const auto& child) {
        return getNodePtr(child)->getName() == path[0];
      });

    if (it == _children.end()) { return nullptr; }

    NodeIf* node = getNodePtr(*it);

    if (path.size() == 1) { return node; }

    if (!node->isDirectory()) { return nullptr; }

    std::vector<std::string> subPath(path.begin() + 1, path.end());
    return static_cast<Directory*>(node)->findNode(subPath);
  }


  void Directory::traverse(const std::function<void(const NodeIf&, size_t)>& visitor, size_t depth) const
  {
    visitor(*this, depth);

    for (const auto& child : _children)
    {
      NodeIf* node = getNodePtr(child);

      if (node->isDirectory()) {
        static_cast<const Directory*>(node)->traverse(visitor, depth + 1);
      }
      else {
        visitor(*node, depth + 1);
      }
    }
  }


  NodeIf* Directory::resolvePath(std::string_view pathStr, const Directory& currentDir) const
  {
    // Find the actual root by walking up the tree
    const NodeIf* rootNode = this;
    while (rootNode->getParent() != nullptr) {
      rootNode = rootNode->getParent();
    }

    // First remove const, then cast to Directory*
    Directory* rootDir = const_cast<Directory*>(static_cast<const Directory*>(rootNode));
    PathResolver resolver(*rootDir);
    return resolver.resolveFromString(pathStr, currentDir);
  }
  

  Path Directory::getRelativePath(const NodeIf& node) const
  {
    Path nodePath = PathResolver::getAbsolutePath(node);
    Path myPath = PathResolver::getAbsolutePath(*this);
    return nodePath.relativeTo(myPath);
  }


  void Directory::checkNameCollision(const std::string& name) const
  {
    auto it = std::find_if(_children.begin(), _children.end(),
      [&name, this](const auto& child) {
        return getNodePtr(child)->getName() == name;
      });

    assert(it == _children.end() && "Name collision in directory");
  }

}
