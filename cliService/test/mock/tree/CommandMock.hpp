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
    explicit CommandMock();
    MOCK_METHOD(void, execute, (const std::vector<std::string>&), (override));
  };

}
