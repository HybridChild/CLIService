#pragma once
#include <string>
#include <string_view>

namespace cliService
{

  enum class CommandStatus
  {
    Success,
    Error,
    InvalidArguments
  };

  class Response
  {
  public:
    explicit Response(std::string msg = "", CommandStatus status = CommandStatus::Success)
      : _message(std::move(msg))
      , _status(status)
      , _showPrompt(true)
    {}

    explicit Response(std::string_view msg = "", CommandStatus status = CommandStatus::Success)
      : _message(msg)
      , _status(status)
      , _showPrompt(true)
    {}

    static Response success(const std::string& msg = "") {
      return Response(msg, CommandStatus::Success);
    }

    static Response error(const std::string& msg) {
      return Response(msg, CommandStatus::Error);
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
