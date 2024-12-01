#include "cliService/parser/requests/ActionRequest.hpp"
#include <sstream>
#include <cassert>

namespace cliService
{

  ActionRequest::ActionRequest(std::string inputStr, Trigger trigger)
    : _absolutePath(false)
    , _trigger(trigger)
  {
    // Empty string check
    if (inputStr.empty())
    {
      _absolutePath = false;
      return;
    }

    // Check if path is absolute
    _absolutePath = (inputStr[0] == '/');

    // Use stringstream to split input
    std::stringstream ss(inputStr);
    std::string token;

    // Get the first token (path)
    ss >> token;

    // Check for invalid trailing slash with arguments
    if (token.length() > 1 && token.back() == '/')
    {
      // If there are more tokens after a path with trailing slash, that's invalid
      std::string nextToken;
      ss >> nextToken;
      assert(nextToken.empty() && "Paths with arguments must not end with a slash");
    }

    // Remove trailing slash if present (for valid cases)
    if (token.length() > 1 && token.back() == '/')
    {
      token.pop_back();
    }

    // Split path into components
    std::stringstream pathStream(token);
    std::string pathComponent;
    
    // Skip the first empty component for absolute paths
    if (_absolutePath)
    {
      pathStream.get();
    }

    // Parse path components
    while (std::getline(pathStream, pathComponent, '/'))
    {
      if (!pathComponent.empty())
      {
        _path.push_back(pathComponent);
      }
    }

    // Parse remaining arguments
    while (ss >> token)
    {
      _args.push_back(token);
    }
  }

  const std::vector<std::string>& ActionRequest::getPath() const
  {
    return _path;
  }

  const std::vector<std::string>& ActionRequest::getArgs() const
  {
    return _args;
  }

  bool ActionRequest::isAbsolutePath() const
  {
    return _absolutePath;
  }

  ActionRequest::Trigger ActionRequest::getTrigger() const
  {
    return _trigger;
  }

}
