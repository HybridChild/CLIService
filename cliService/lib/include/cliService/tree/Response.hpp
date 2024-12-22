#pragma once
#include <string>
#include <string_view>

namespace cliService
{

  enum class ResponseStatus
  {
    Success,
    Error,
    InvalidArguments,
    AccessDenied
  };

  class Response
  {
  public:
    explicit Response(std::string msg = "", ResponseStatus status = ResponseStatus::Success)
      : _message(std::move(msg))
      , _status(status)
      , _showPrompt(true)
    {}

    explicit Response(std::string_view msg = "", ResponseStatus status = ResponseStatus::Success)
      : _message(msg)
      , _status(status)
      , _showPrompt(true)
    {}

    static Response success(const std::string& msg = "") {
      return Response(msg, ResponseStatus::Success);
    }

    static Response error(const std::string& msg) {
      return Response(msg, ResponseStatus::Error);
    }

    const std::string& getMessage() const { return _message; }
    ResponseStatus getStatus() const { return _status; }
    bool shouldShowPrompt() const { return _showPrompt; }
    void setShowPrompt(bool show) { _showPrompt = show; }

  private:
    std::string _message;
    ResponseStatus _status;
    bool _showPrompt;
  };

}
