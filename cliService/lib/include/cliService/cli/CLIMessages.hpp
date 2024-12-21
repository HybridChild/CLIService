#pragma once
#include <string>
#include <string_view>

namespace cliService
{

  class CLIMessages
  {
  public:
    CLIMessages() = default;

    // Setters for customizing messages
    void setWelcomeMessage(std::string msg) { _welcomeMessage = std::move(msg); }
    void setLoggedInMessage(std::string msg) { _loggedInMessage = std::move(msg); }
    void setLoggedOutMessage(std::string msg) { _loggedOutMessage = std::move(msg); }
    void setExitMessage(std::string msg) { _exitMessage = std::move(msg); }
    void setNoArgumentsMessage(std::string msg) { _noArgumentsMessage = std::move(msg); }
    void setAccessDeniedMessage(std::string msg) { _accessDeniedMessage = std::move(msg); }
    void setInvalidPathMessage(std::string msg) { _invalidPathMessage = std::move(msg); }
    void setInvalidLoginMessage(std::string msg) { _invalidLoginMessage = std::move(msg); }

    // Getters that return string_view to avoid copies
    std::string_view getWelcomeMessage() const { return _welcomeMessage; }
    std::string_view getLoggedInMessage() const { return _loggedInMessage; }
    std::string_view getLoggedOutMessage() const { return _loggedOutMessage; }
    std::string_view getExitMessage() const { return _exitMessage; }
    std::string_view getNoArgumentsMessage() const { return _noArgumentsMessage; }
    std::string_view getAccessDeniedMessage() const { return _accessDeniedMessage; }
    std::string_view getInvalidPathMessage() const { return _invalidPathMessage; }
    std::string_view getInvalidLoginMessage() const { return _invalidLoginMessage; }

    // Static method to get default messages
    static CLIMessages getDefaults()
    {
      CLIMessages messages;
      messages.setWelcomeMessage("Welcome to CLI Service. Please login.");
      messages.setLoggedInMessage("Logged in. Type 'help' for help.");
      messages.setLoggedOutMessage("Logged out.");
      messages.setExitMessage("Exiting CLI Service.");
      messages.setNoArgumentsMessage("Command takes no arguments.");
      messages.setAccessDeniedMessage("Access denied");
      messages.setInvalidPathMessage("Invalid path");
      messages.setInvalidLoginMessage("Invalid login attempt. Please enter <username>:<password>");
      return messages;
    }

  private:
    std::string _welcomeMessage;
    std::string _loggedInMessage;
    std::string _loggedOutMessage;
    std::string _exitMessage;
    std::string _noArgumentsMessage;
    std::string _accessDeniedMessage;
    std::string _invalidPathMessage;
    std::string _invalidLoginMessage;
  };

}
