#pragma once
#include <string>
#include <string_view>

namespace cliService
{

  class CLIResponse
  {
  public:

    enum class Status
    {
      Success,
      Error,
      InvalidArguments,
      InvalidPath,
      AccessDenied
    };

    explicit CLIResponse(std::string msg = "", Status status = Status::Success)
      : _message(std::move(msg))
      , _status(status)
      , _showPrompt(true)
      , _indentMessage(true)
      , _inlineMessage(false)
      , _prefixNewLine(true)
      , _postfixNewLine(true)
    {}

    explicit CLIResponse(std::string_view msg = "", Status status = Status::Success)
      : _message(msg)
      , _status(status)
      , _showPrompt(true)
      , _indentMessage(true)
      , _inlineMessage(false)
      , _prefixNewLine(true)
      , _postfixNewLine(true)
    {}

    static CLIResponse success(const std::string& msg = "") { return CLIResponse(msg, Status::Success); }
    static CLIResponse success(std::string_view msg) { return CLIResponse(msg, Status::Success); }
    static CLIResponse error(const std::string& msg) { return CLIResponse(msg, Status::Error); }
    static CLIResponse error(std::string_view msg) { return CLIResponse(msg, Status::Error); }

    const std::string& getMessage() const { return _message; }
    void appendToMessage(const std::string& msg) { _message += msg; }
    void appendToMessage(std::string_view msg) { _message += msg; }

    Status getStatus() const { return _status; }
    bool showPrompt() const { return _showPrompt; }
    bool indentMessage() const { return _indentMessage; }
    bool inlineMessage() const { return _inlineMessage; }
    bool prefixNewLine() const { return _prefixNewLine; }
    bool postfixNewLine() const { return _postfixNewLine; }

    void setStatus(Status status) { _status = status; }
    void setShowPrompt(bool show) { _showPrompt = show; }
    void setIndentMessage(bool indent) { _indentMessage = indent; }
    void setInlineMessage(bool inlineMsg) { _inlineMessage = inlineMsg; }
    void setPrefixNewLine(bool prefix) { _prefixNewLine = prefix; }
    void setPostfixNewLine(bool postfix) { _postfixNewLine = postfix; }

  private:
    std::string _message;
    Status _status;
    bool _showPrompt;
    bool _indentMessage;
    bool _inlineMessage;
    bool _prefixNewLine;
    bool _postfixNewLine;
  };

}
