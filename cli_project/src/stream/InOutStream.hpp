#pragma once

#include <string>
#include <iostream>

class InOutStream {
  public:
  std::string read() {
    std::string input;
    std::getline(std::cin, input);
    return input;
  }

  void write(const std::string& data) {
    std::cout << data << std::endl;
  }
};
