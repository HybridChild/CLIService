#pragma once
#include <cstdint>
#include <string_view>

namespace cliService
{

  class CharIOStreamIf
  {
  public:
    virtual ~CharIOStreamIf() = default;

    virtual bool putChar(char c) = 0;
    virtual bool getChar(char& c) = 0;
    virtual bool getCharTimeout(char& c, uint32_t timeout_ms) = 0;

    virtual bool putString(std::string_view str)
    {
      for (char c : str) {
        if (!putChar(c)) { return false; }
      }

      return true;
    }

    virtual bool available() const = 0;
    virtual void flush() = 0;
    virtual bool isOpen() const = 0;
    virtual bool hasError() const = 0;
    virtual const char* getLastError() const = 0;
    virtual void clearError() = 0;

  protected:
    CharIOStreamIf() = default;
    CharIOStreamIf(const CharIOStreamIf&) = delete;
    CharIOStreamIf& operator=(const CharIOStreamIf&) = delete;
  };

}
