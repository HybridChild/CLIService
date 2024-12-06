#pragma once
#include <string>

namespace util
{

  bool isIntegerString(const std::string& str)
  {
    if (str.empty() || str.length() > 3) return false;

    // Check if all characters are digits
    for (char c : str)
    {
      if (!std::isdigit(c)) return false;
    }

    return true;
  }

}
