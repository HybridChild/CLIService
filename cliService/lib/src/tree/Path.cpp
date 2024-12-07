#include "cliService/tree/Path.hpp"
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
    
    if (pathStr.empty())
    {
      isAbsolute = false;
      return components;
    }

    isAbsolute = (pathStr[0] == '/');

    size_t pos = isAbsolute ? 1 : 0;
    size_t start = pos;

    while (pos <= pathStr.length())
    {
      if (pos == pathStr.length() || pathStr[pos] == '/')
      {
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

    for (const auto& component : components)
    {
      // Skip empty components and "."
      if (component.empty() || component == ".") { continue; }

      // Handle ".." by removing last component if possible
      if (component == "..")
      {
        if (!normalized.empty()) { normalized.pop_back(); }
        // Keep ".." if we're at the start of a relative path
        else if (!isAbsolute) { normalized.push_back(".."); }
      }
      else
      {
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
    if (isEmpty()) { return Path({".."}, _isAbsolute); }

    auto parentComponents = _components;
    parentComponents.pop_back();
    return Path(std::move(parentComponents), _isAbsolute);
  }

  Path Path::join(const Path& other) const
  {
    // If other is absolute, return it
    if (other.isAbsolute()) { return other; }

    // Combine components
    auto newComponents = _components;
    newComponents.insert(newComponents.end(), other.components().begin(), other.components().end());

    return Path(std::move(newComponents), _isAbsolute);
  }

  // Convert this path to be relative to another path
  Path Path::relativeTo(const Path& base) const
  {
    // Both paths must be absolute for this to work
    assert(_isAbsolute && base._isAbsolute);
    
    // Find common prefix
    size_t commonPrefix = 0;
    while (commonPrefix < _components.size() && 
            commonPrefix < base._components.size() &&
            _components[commonPrefix] == base._components[commonPrefix])
    {
      commonPrefix++;
    }
    
    // Build relative path
    std::vector<std::string> relativeComponents;
    
    // Add ".." for each component in base after common prefix
    for (size_t i = commonPrefix; i < base._components.size(); i++)
    {
      relativeComponents.push_back("..");
    }
    
    // Add remaining components from this path
    relativeComponents.insert(relativeComponents.end(), _components.begin() + commonPrefix, _components.end());
        
    return Path(std::move(relativeComponents), false);
  }

  std::string Path::toString() const
  {
    if (isEmpty()) { return _isAbsolute ? "/" : "."; }

    std::ostringstream oss;
    if (_isAbsolute) { oss << '/'; }

    for (size_t i = 0; i < _components.size(); ++i)
    {
      if (i > 0) { oss << '/'; }
      oss << _components[i];
    }

    return oss.str();
  }

  bool Path::operator==(const Path& other) const
  {
    return _isAbsolute == other._isAbsolute && _components == other._components;
  }

}
