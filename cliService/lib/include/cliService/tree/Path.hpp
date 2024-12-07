#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cliService
{

  class Path 
  {
  public:
    Path() : _isAbsolute(false) {}
    explicit Path(std::string_view pathStr);
    Path(std::vector<std::string> components, bool isAbsolute);
    
    bool isAbsolute() const { return _isAbsolute; }
    bool isEmpty() const { return _components.empty(); }
    const std::vector<std::string>& components() const { return _components; }

    Path normalized() const;
    Path parent() const;
    Path join(const Path& other) const;
    Path relativeTo(const Path& base) const;
    
    std::string toString() const;

    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const { return !(*this == other); }

  private:
    std::vector<std::string> _components;
    bool _isAbsolute;

    static std::vector<std::string> parseComponents(std::string_view pathStr, bool& isAbsolute);
    static std::vector<std::string> normalizeComponents(const std::vector<std::string>& components, bool isAbsolute);
  };

}
