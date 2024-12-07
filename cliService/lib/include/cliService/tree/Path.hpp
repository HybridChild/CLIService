#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cliService
{

  class Path 
  {
  public:
    // Constructs empty relative path
    Path() : _isAbsolute(false) {}
    
    // Construct from string view
    explicit Path(std::string_view pathStr);
    
    // Construct from components
    Path(std::vector<std::string> components, bool isAbsolute);
    
    // Basic properties
    bool isAbsolute() const { return _isAbsolute; }
    bool isEmpty() const { return _components.empty(); }
    const std::vector<std::string>& components() const { return _components; }
    
    // Path operations
    Path normalized() const;
    Path parent() const;
    Path joined(const Path& other) const;
    
    // String conversion
    std::string toString() const;
    
    // Comparison operators
    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const { return !(*this == other); }

  private:
    std::vector<std::string> _components;
    bool _isAbsolute;
    
    static std::vector<std::string> parseComponents(std::string_view pathStr, bool& isAbsolute);
    static std::vector<std::string> normalizeComponents(const std::vector<std::string>& components, bool isAbsolute);
  };

}
