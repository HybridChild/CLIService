#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "gmock/gmock.h"
#include <vector>
#include <string>

namespace cliService 
{

  enum class AccessLevel 
  {
    User,
    Admin
  };

  class CommandMock : public CommandIf 
  {
  public:
    explicit CommandMock(std::string name, AccessLevel level)
      : CommandIf(name, level)
    {}
    
    MOCK_METHOD(CommandResponse, execute, (const std::vector<std::string>&), (override));
  };

}
