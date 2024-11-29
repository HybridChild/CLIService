#pragma once

#include <string>

namespace cliService {

  enum class AccessLevel;

  class MenuItemIf {
  public:
    virtual ~MenuItemIf() = default;

    std::string getName() const { return _name; }
    AccessLevel getAccessLevel() const { return _accessLevel; }

  protected:
    MenuItemIf(const std::string& name, AccessLevel accessLevel)
      : _name(name), _accessLevel(accessLevel)
    {}

  private:
    MenuItemIf() = delete;
    
    std::string _name;
    AccessLevel _accessLevel;
  };
}