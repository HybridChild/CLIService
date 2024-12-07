#include "cliService/tree/Path.hpp"
#include <cassert>
#include <sstream>

namespace cliService
{

  Path::Path(std::string_view pathStr)
  {
    bool isAbsolute;
    _elements = parseElements(pathStr, isAbsolute);
    _isAbsolute = isAbsolute;
  }

  Path::Path(std::vector<std::string> elements, bool isAbsolute)
    : _elements(std::move(elements))
    , _isAbsolute(isAbsolute)
  {}

  std::vector<std::string> Path::parseElements(std::string_view pathStr, bool& isAbsolute)
  {
    std::vector<std::string> elements;
    
    if (pathStr.empty())
    {
      isAbsolute = false;
      return elements;
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
          elements.push_back(std::string(pathStr.substr(start, pos - start)));
        }

        start = pos + 1;
      }
      pos++;
    }

    return elements;
  }

  std::vector<std::string> Path::normalizeElements(const std::vector<std::string>& elements, bool isAbsolute)
  {
    std::vector<std::string> normalized;
    normalized.reserve(elements.size());

    for (const auto& element : elements)
    {
      // Skip empty elements and "."
      if (element.empty() || element == ".") { continue; }

      // Handle ".." by removing last elements if possible
      if (element == "..")
      {
        if (!normalized.empty()) { normalized.pop_back(); }
        // Keep ".." if we're at the start of a relative path
        else if (!isAbsolute) { normalized.push_back(".."); }
      }
      else
      {
        normalized.push_back(element);
      }
    }

    return normalized;
  }

  Path Path::normalized() const
  {
    return Path(normalizeElements(_elements, _isAbsolute), _isAbsolute);
  }

  Path Path::parent() const
  {
    if (isEmpty()) { return Path({".."}, _isAbsolute); }

    auto parentElements = _elements;
    parentElements.pop_back();
    return Path(std::move(parentElements), _isAbsolute);
  }

  Path Path::join(const Path& other) const
  {
    // If other is absolute, return it
    if (other.isAbsolute()) { return other; }

    // Combine elements
    auto newElements = _elements;
    newElements.insert(newElements.end(), other.elements().begin(), other.elements().end());

    return Path(std::move(newElements), _isAbsolute);
  }

  // Convert this path to be relative to another path
  Path Path::relativeTo(const Path& base) const
  {
    // Both paths must be absolute for this to work
    assert(_isAbsolute && base._isAbsolute);
    
    // Find common prefix
    size_t commonPrefix = 0;
    while (commonPrefix < _elements.size() && 
            commonPrefix < base._elements.size() &&
            _elements[commonPrefix] == base._elements[commonPrefix])
    {
      commonPrefix++;
    }
    
    // Build relative path
    std::vector<std::string> relativeElements;
    
    // Add ".." for each element in base after common prefix
    for (size_t i = commonPrefix; i < base._elements.size(); i++)
    {
      relativeElements.push_back("..");
    }
    
    // Add remaining elements from this path
    relativeElements.insert(relativeElements.end(), _elements.begin() + commonPrefix, _elements.end());
        
    return Path(std::move(relativeElements), false);
  }

  std::string Path::toString() const
  {
    if (isEmpty()) { return _isAbsolute ? "/" : "."; }

    std::ostringstream oss;
    if (_isAbsolute) { oss << '/'; }

    for (size_t i = 0; i < _elements.size(); ++i)
    {
      if (i > 0) { oss << '/'; }
      oss << _elements[i];
    }

    return oss.str();
  }

  bool Path::operator==(const Path& other) const
  {
    return _isAbsolute == other._isAbsolute && _elements == other._elements;
  }

}
