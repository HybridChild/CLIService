#include "mock/tree/CommandMock.hpp" 
#include "cliService/tree/CommandIf.hpp"

namespace cliService
{

  CommandMock::CommandMock() 
    : CommandIf("test", AccessLevel::Admin)
  {}

}
