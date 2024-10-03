#include "RgbLedCommand.hpp"

void RgbLedCommand::execute(CommandRequest& request) {
  const auto& args = request.getArgs();
  if (args.size() != 3) {
    request.setResponse("Error: rgbLed command requires 3 arguments. Usage: " + getUsage() + "\n", 1);
    return;
  }
  try {
    int r = std::stoi(args[0]);
    int g = std::stoi(args[1]);
    int b = std::stoi(args[2]);
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
      throw std::out_of_range("RGB values must be between 0 and 255\n");
    }
    // Simulate setting RGB LED
    request.setResponse("RGB LED set to " + args[0] + " " + args[1] + " " + args[2] + "\n", 0);
  } catch (const std::exception& e) {
    request.setResponse("Error: Invalid RGB values. " + std::string(e.what()) + "\n", 1);
  }
}

std::string RgbLedCommand::getName() const { return "rgbLed"; }
std::string RgbLedCommand::getUsage() const { return "rgbLed <red> <green> <blue>"; }
