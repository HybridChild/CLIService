#include "cliService/tree/PathResolver.hpp"
#include <cassert>

namespace cliService
{

  NodeIf* PathResolver::resolveFromString(std::string_view pathStr, const Directory& currentDir) const 
  {
    return resolve(Path(pathStr), currentDir);
  }

  NodeIf* PathResolver::resolve(const Path& path, const Directory& currentDir) const 
  {
    // Normalize the path first
    Path normalizedPath = path.normalized();
    
    // Handle absolute vs relative paths
    if (normalizedPath.isAbsolute()) {
      return resolveAbsolute(normalizedPath);
    }
    
    // Empty relative path returns current directory
    if (normalizedPath.isEmpty()) {
      return const_cast<Directory*>(&currentDir);
    }
    
    // Start from current directory for relative paths
    NodeIf* current = const_cast<Directory*>(&currentDir);
    
    // Process each component
    for (const auto& component : normalizedPath.components()) {
      // Must be in a directory to continue
      if (!current || !current->isDirectory()) {
        return nullptr;
      }
      
      if (component == "..") {
        current = current->getParent();
        // If we hit root's parent, stay at root
        if (!current) {
          current = &_root;
        }
      } else {
        auto* dir = static_cast<Directory*>(current);
        current = dir->findNode({component});
        if (!current) {
          return nullptr;
        }
      }
    }
    
    return current;
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
    
    // Process each component
    for (const auto& component : path.components()) {
      if (!current->isDirectory()) {
        return nullptr;
      }
      
      auto* dir = static_cast<Directory*>(current);
      current = dir->findNode({component});
      
      if (!current) {
        return nullptr;
      }
    }
    
    return current;
  }

  Path PathResolver::getAbsolutePath(const NodeIf& node) 
  {
    std::vector<std::string> components;
    
    // Walk up the tree collecting node names
    const NodeIf* current = &node;
    while (current->getParent()) {
      components.push_back(current->getName());
      current = current->getParent();
    }
    
    // Reverse the components since we collected them from leaf to root
    std::reverse(components.begin(), components.end());
    
    // Create absolute path
    return Path(std::move(components), true);
  }

}
