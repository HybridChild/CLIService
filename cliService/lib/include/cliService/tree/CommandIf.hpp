#pragma once
#include "cliService/tree/NodeIf.hpp"
#include "cliService/tree/CommandResponse.hpp"
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

    virtual CommandResponse execute(const std::vector<std::string>& args) = 0;

    bool isDirectory() const override { return false; }
    const std::string& getDescription() const { return _description; }

  private:
    std::string _description;
  };

}
