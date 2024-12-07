#include "cliService/tree/Directory.hpp"
#include "cliService/tree/PathResolver.hpp"
#include <algorithm>

namespace cliService
{

  Directory::Directory(std::string name, AccessLevel level)
    : NodeIf(std::move(name), level)
  {}

  bool Directory::isDirectory() const
  {
    return true;
  }

  Directory& Directory::addDirectory(const std::string& name, AccessLevel level)
  {
    auto dir = std::make_unique<Directory>(name, level);
    Directory* dirPtr = dir.get();
    addChild(std::move(dir));
    return *dirPtr;
  }

  NodeIf* Directory::findNode(const std::vector<std::string>& path) const
  {
    // Base case: empty path returns current directory
    if (path.empty()) { return const_cast<Directory*>(this); }

    // Search for child matching first path element
    auto it = std::find_if(_children.begin(), _children.end(),
      [&path](const auto& child) {
        return child->getName() == path[0];
      });

    // No matching child found
    if (it == _children.end()) { return nullptr; }

    // If path has only one element, return the found node
    if (path.size() == 1) { return it->get(); }

    // If more path elements exist but found node isn't a directory, navigation fails
    if (!(*it)->isDirectory()) { return nullptr; }

    // Recursive case: continue search in found directory with remaining path
    std::vector<std::string> subPath(path.begin() + 1, path.end());
    return static_cast<Directory*>(it->get())->findNode(subPath);
  }

  void Directory::traverse(const std::function<void(const NodeIf&, int)>& visitor, int depth) const
  {
    // Visit current directory
    visitor(*this, depth);

    // Visit all children
    for (const auto& child : _children)
    {
      if (child->isDirectory())
      {
        // Recursively traverse directories
        static_cast<const Directory*>(child.get())->traverse(visitor, depth + 1);
      }
      else
      {
        // Visit commands directly
        visitor(*child, depth + 1);
      }
    }
  }

  NodeIf* Directory::resolvePath(std::string_view pathStr, const Directory& currentDir) const
  {
    // Create a resolver using this directory as root
    PathResolver resolver(*const_cast<Directory*>(this));
    return resolver.resolveFromString(pathStr, currentDir);
  }

  Path Directory::getRelativePath(const NodeIf& node) const
  {
    // Get absolute paths for both nodes
    Path nodePath = PathResolver::getAbsolutePath(node);
    Path myPath = PathResolver::getAbsolutePath(*this);
    
    // Convert absolute path difference to relative
    // This would require adding a new method to Path class
    return nodePath.relativeTo(myPath);
  }

  void Directory::addChild(std::unique_ptr<NodeIf> child)
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
