#include "cliService/tree/PathResolver.hpp"
#include <algorithm>
#include <cassert>

namespace cliService
{

  PathResolver::PathResolver(Directory& root) 
    : _root(root)
  {}


  NodeIf* PathResolver::resolveFromString(std::string_view pathStr, const Directory& currentDir) const {
    return resolve(Path(pathStr), currentDir);
  }


  NodeIf* PathResolver::resolve(const Path& path, const Directory& currentDir) const 
  {
    // For relative paths, first convert to absolute by combining with current directory path
    if (!path.isAbsolute())
    {
      // Get the absolute path of current directory
      Path currentPath = getAbsolutePath(currentDir);

      // Join the paths and normalize
      Path absolutePath = currentPath.join(path).normalized();
      return resolveAbsolute(absolutePath);
    }

    // Handle absolute paths directly
    return resolveAbsolute(path.normalized());
  }


  NodeIf* PathResolver::resolveAbsolute(const Path& path) const 
  {
    assert(path.isAbsolute());

    // Empty absolute path or just "/" returns root
    if (path.isEmpty()) { 
        return &_root; 
    }

    // Start from root
    NodeIf* current = &_root;

    // Process each element of the normalized path
    for (const auto& element : path.elements())
    {
      if (!current->isDirectory()) { 
        return nullptr; 
      }

      auto* dir = static_cast<Directory*>(current);

      current = dir->findNode({element});
      if (!current) { 
        return nullptr;
      }
    }

    return current;
  }

  Path PathResolver::getAbsolutePath(const NodeIf& node) 
  {
    std::vector<std::string> elements;

    // Walk up the tree collecting node names
    const NodeIf* current = &node;

    while (current->getParent())
    {
      elements.push_back(current->getName());
      current = current->getParent();
    }

    // Reverse the elements since we collected them from leaf to root
    std::reverse(elements.begin(), elements.end());

    // Create absolute path
    return Path(std::move(elements), true);
  }

}
