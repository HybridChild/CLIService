#include "cliService/cli/ActionRequest.hpp"
#include <cassert>
#include <sstream>
#include <istream>
#include <string>

namespace cliService
{

  void ActionRequest::parseInput(std::string_view input, std::string_view& pathStr, std::string_view& argsStr)
  {
    // Find first space that separates path from args
    size_t spacePos = input.find(' ');
    
    if (spacePos == std::string_view::npos)
    {
      // No args, entire input is path
      pathStr = input;
      argsStr = std::string_view();
    }
    else
    {
      // Split into path and args
      pathStr = input.substr(0, spacePos);
      
      // Skip spaces between path and args
      size_t argsStart = spacePos + 1;
      while (argsStart < input.length() && input[argsStart] == ' ')
      {
        argsStart++;
      }
      
      argsStr = input.substr(argsStart);
    }
    
    // Validate path doesn't end with slash when args present
    if (!argsStr.empty())
    {
      assert(pathStr.empty() || pathStr.back() != '/' && "Paths with arguments must not end with a slash");
    }
  }

  ActionRequest::ActionRequest(std::string_view inputStr, Trigger trigger)
    : _trigger(trigger)
  {
    // Split input into path and args
    std::string_view pathStr, argsStr;
    parseInput(inputStr, pathStr, argsStr);
    
    // Create path object
    _path = Path(pathStr);
    
    // Parse args if present
    if (!argsStr.empty())
    {
      // Fix for most vexing parse - use brace initialization
      std::istringstream argStream{std::string(argsStr)};
      std::string arg;

      while (argStream >> arg) {
        _args.push_back(std::move(arg));
      }
    }
  }

}
