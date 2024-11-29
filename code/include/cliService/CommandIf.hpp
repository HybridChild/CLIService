#pragma once

#include <string>
#include "cliService/MenuItemIf.hpp"
#include "cliService/CommandRequest.hpp"

namespace cliService {

  enum class AccessLevel;

  class CommandIf : public MenuItemIf{
  public:
    CommandIf(const std::string& name, AccessLevel accessLevel)
      : MenuItemIf(name, accessLevel)
    {}

    virtual ~CommandIf() = default;

    virtual void execute(const CommandRequest& request, std::string& response) = 0;
    virtual std::string getUsage() const = 0;
  };

}
