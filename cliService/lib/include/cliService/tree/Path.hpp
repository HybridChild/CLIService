#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cliService
{

  class Path 
  {
  public:
    explicit Path(std::string_view pathStr);
    Path();
    Path(std::vector<std::string> elements, bool isAbsolute);
    
    bool isAbsolute() const { return _isAbsolute; }
    bool isEmpty() const { return _elements.empty(); }
    const std::vector<std::string>& elements() const { return _elements; }

    Path normalized() const;
    Path parent() const;
    Path join(const Path& other) const;
    Path relativeTo(const Path& base) const;
    
    std::string toString() const;

    bool operator==(const Path& other) const;
    bool operator!=(const Path& other) const { return !(*this == other); }

  private:
    std::vector<std::string> _elements;
    bool _isAbsolute;

    static std::vector<std::string> parseElements(std::string_view pathStr, bool& isAbsolute);
    static std::vector<std::string> normalizeElements(const std::vector<std::string>& elements, bool isAbsolute);
  };

}
