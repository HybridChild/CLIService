#pragma once

#include "../menu/CommandMenuTree.hpp"
#include "../io/InOutStream.hpp"
#include <memory>

class CLIServiceConfiguration {
public:
  CLIServiceConfiguration(
    std::unique_ptr<CommandMenuTree> menuTree,
    std::unique_ptr<InOutStream> ioStream
  ) : menuTree(std::move(menuTree)), ioStream(std::move(ioStream)) {}

  CommandMenuTree* getMenuTree() const { return menuTree.get(); }
  InOutStream* getIOStream() const { return ioStream.get(); }

private:
  std::unique_ptr<CommandMenuTree> menuTree;
  std::unique_ptr<InOutStream> ioStream;
};
