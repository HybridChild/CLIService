#include <stdexcept>
#include "commands/RgbLedCommand.hpp"

namespace cliService {

  void RgbLedCommand::execute(const CommandRequest& request, std::string& response) {
    const auto& args = request.getArgs();
    if (args.size() != 3) {
      response = "Error: rgbLed command requires 3 arguments. Usage: " + getUsage() + "\n";
      return;
    }
    try {
      int r = std::stoi(args[0]);
      int g = std::stoi(args[1]);
      int b = std::stoi(args[2]);
      if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw std::out_of_range("RGB values must be between 0 and 255");
      }
      // Simulate setting RGB LED
      response = "RGB LED set to " + args[0] + " " + args[1] + " " + args[2] + "\n";
    } catch (const std::exception& e) {
      response = "Error: Invalid RGB values. " + std::string(e.what()) + "\n";
    }
  }

  std::string RgbLedCommand::getName() const { return "rgbLed"; }
  std::string RgbLedCommand::getUsage() const { return "rgbLed <red> <green> <blue>"; }

}
