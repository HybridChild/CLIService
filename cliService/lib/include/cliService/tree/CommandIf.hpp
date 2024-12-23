#pragma once
#include "cliService/tree/NodeIf.hpp"
#include "cliService/tree/Response.hpp"
#include <vector>

namespace cliService
{

  class CommandIf : public NodeIf
  {
  public:
    explicit CommandIf(std::string name, AccessLevel level, std::string description = "")
      : NodeIf(std::move(name), level)
      , _description(std::move(description))
    {}

    virtual ~CommandIf() = default;

    virtual Response execute(const std::vector<std::string>& args) = 0;

    bool isDirectory() const override { return false; }
    const std::string& getDescription() const { return _description; }

    static Response createInvalidArgumentCountResponse(size_t expected)
    {
      std::string response;

      if (expected == 0) {
        response += "Command takes no arguments. Try again.";
      }
      else
      {
        std::string argStr = expected == 1 ? "argument" : "arguments";
        response += "Expected " + std::to_string(expected) + " " + argStr + ". Try again.";
      }

      return Response(response, ResponseStatus::InvalidArguments);
    }

  private:
    std::string _description;
  };

}
