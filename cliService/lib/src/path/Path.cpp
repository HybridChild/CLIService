#include "cliService/path/Path.hpp"
#include <cassert>
#include <sstream>

namespace cliService
{

  Path::Path(std::string_view pathStr)
  {
    bool isAbsolute;
    _components = parseComponents(pathStr, isAbsolute);
    _isAbsolute = isAbsolute;
  }

  Path::Path(std::vector<std::string> components, bool isAbsolute)
    : _components(std::move(components))
    , _isAbsolute(isAbsolute)
  {}

  std::vector<std::string> Path::parseComponents(std::string_view pathStr, bool& isAbsolute)
  {
    std::vector<std::string> components;
    
    // Empty path
    if (pathStr.empty()) {
      isAbsolute = false;
      return components;
    }
    
    // Check if absolute
    isAbsolute = (pathStr[0] == '/');
    
    // Current position and component start
    size_t pos = isAbsolute ? 1 : 0;
    size_t start = pos;
    
    // Parse components
    while (pos <= pathStr.length())
    {
      if (pos == pathStr.length() || pathStr[pos] == '/')
      {
        // Extract component if we have one
        if (pos > start)
        {
          components.push_back(std::string(pathStr.substr(start, pos - start)));
        }

        start = pos + 1;
      }
      pos++;
    }
    
    return components;
  }

  std::vector<std::string> Path::normalizeComponents(const std::vector<std::string>& components, bool isAbsolute)
  {
    std::vector<std::string> normalized;
    normalized.reserve(components.size());
    
    for (const auto& component : components) {
      // Skip empty components and "."
      if (component.empty() || component == ".") {
        continue;
      }
      
      // Handle ".." by removing last component if possible
      if (component == "..") {
        if (!normalized.empty()) {
          normalized.pop_back();
        }
        // Keep ".." if we're at the start of a relative path
        else if (!isAbsolute) {
          normalized.push_back("..");
        }
      } else {
        normalized.push_back(component);
      }
    }
    
    return normalized;
  }

  Path Path::normalized() const
  {
    return Path(normalizeComponents(_components, _isAbsolute), _isAbsolute);
  }

  Path Path::parent() const
  {
    if (isEmpty()) {
      return Path({".."}, _isAbsolute);
    }
    
    auto parentComponents = _components;
    parentComponents.pop_back();
    return Path(std::move(parentComponents), _isAbsolute);
  }

  Path Path::joined(const Path& other) const
  {
    // If other is absolute, return it
    if (other.isAbsolute())
    {
      return other;
    }
    
    // Combine components
    auto newComponents = _components;
    newComponents.insert(newComponents.end(), 
      other.components().begin(), 
      other.components().end());
    
    return Path(std::move(newComponents), _isAbsolute);
  }

  std::string Path::toString() const
  {
    if (isEmpty())
    {
      return _isAbsolute ? "/" : ".";
    }
    
    std::ostringstream oss;
    if (_isAbsolute) {
      oss << '/';
    }
    
    for (size_t i = 0; i < _components.size(); ++i)
    {
      if (i > 0)
      {
        oss << '/';
      }
      oss << _components[i];
    }
    
    return oss.str();
  }

  bool Path::operator==(const Path& other) const
  {
    return _isAbsolute == other._isAbsolute && _components == other._components;
  }

}
