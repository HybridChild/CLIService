#pragma once
#include "cliService/tree/Path.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/NodeIf.hpp"
#include <cassert>

namespace cliService
{

  class PathResolver
  {
  public:
    explicit PathResolver(Directory& root);

    // Core resolution method - returns nullptr if path cannot be resolved
    NodeIf* resolve(const Path& path, const Directory& currentDir) const;

    // Convenience methods for string paths
    NodeIf* resolveFromString(std::string_view pathStr, const Directory& currentDir) const;

    // Get the absolute path of a node
    static Path getAbsolutePath(const NodeIf& node);

  private:
    Directory& _root;

    // Internal resolution methods
    NodeIf* resolveAbsolute(const Path& path) const;
  };

}
