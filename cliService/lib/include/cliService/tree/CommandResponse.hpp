#pragma once
#include <string>

namespace cliService
{

  enum class CommandStatus
  {
    Success,
    Error,
    InvalidArguments
  };

  class CommandResponse
  {
  public:
    explicit CommandResponse(std::string msg = "", CommandStatus status = CommandStatus::Success)
      : _message(std::move(msg))
      , _status(status)
      , _showPrompt(true)
    {}

    static CommandResponse success(const std::string& msg = "") {
      return CommandResponse(msg, CommandStatus::Success);
    }
    
    static CommandResponse error(const std::string& msg) {
      return CommandResponse(msg, CommandStatus::Error);
    }

    const std::string& getMessage() const { return _message; }
    CommandStatus getStatus() const { return _status; }
    bool shouldShowPrompt() const { return _showPrompt; }
    void setShowPrompt(bool show) { _showPrompt = show; }

  private:
    std::string _message;
    CommandStatus _status;
    bool _showPrompt;
  };

}
