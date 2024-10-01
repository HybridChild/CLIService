#pragma once

#include "../stream/InOutStream.hpp"
#include "../menu/CommandMenuTree.hpp"

class CLIServiceConfiguration {
public:
  CLIServiceConfiguration(std::unique_ptr<CommandMenuTree> tree, std::unique_ptr<InOutStream> inOutStream) 
    : menuTree(std::move(tree)), inOutStream(std::move(inOutStream))
  {}

  std::unique_ptr<CommandMenuTree> menuTree;
  std::unique_ptr<InOutStream> inOutStream;
};
