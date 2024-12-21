#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"
#include "util/util.hpp"
#include <iomanip>
#include <sstream>


namespace cliService
{

  class PotmeterGetCommand : public CommandIf
  {
  public:
    PotmeterGetCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, "Get potmeter value - Args: <pot ID>")
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() != 1) {
        return CommandIf::createInvalidArgumentCountResponse(1);
      }

      if (!util::isIntegerString(args[0])) {
        return CommandResponse("Invalid potmeter ID: " + args[0] + ". Must be integer.", CommandStatus::InvalidArguments);
      }

      uint32_t potId = std::stoi(args[0]);

      if (potId < 1 || potId > 4) {
        return CommandResponse("Invalid potmeter ID: " + args[0], CommandStatus::InvalidArguments);
      }

      uint32_t potmeterValue = readPotmeter(potId);
      float potmeterPercentage = calcPotmeterPercentage(potmeterValue);

      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << potmeterPercentage;
      std::string potPercentStr = ss.str();

      return CommandResponse::success("Potmeter " + args[0] + " value: " + std::to_string(potmeterValue) + ", " + potPercentStr + "%");
    }

  private:
    uint32_t readPotmeter(uint32_t id) const
    {
      uint32_t value = 0;

      // read potmeter value from hardware
      switch (id)
      {
        case 1:
          value = 123;
          break;
        case 2:
          value = 255;
          break;
        case 3:
          value = 666;
          break;
        case 4:
          value = 1023;
          break;
        default:
          break;
      }

      return value;
    }

    float calcPotmeterPercentage(uint32_t value) const { return value / POTMETER_MAX * 100; }
    static constexpr uint32_t POTMETER_MAX = 1023;
  };

}
