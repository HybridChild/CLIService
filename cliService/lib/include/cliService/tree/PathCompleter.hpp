#pragma once
#include "cliService/tree/Path.hpp"
#include "cliService/tree/PathResolver.hpp"
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

namespace cliService
{

  class PathCompleter
  {
  public:

    struct CompletionResult
    {
      std::string fullPath;
      std::string matchedNode;
      std::string fillCharacters;
      std::vector<std::string> allOptions;
      bool isDirectory;
    };

    static CompletionResult complete(const Directory& currentDir, std::string_view partialInput, AccessLevel accessLevel)
    {
      if (partialInput.empty()) {
        return listCurrentDirectory(currentDir, accessLevel);
      }

      Path partialPath(partialInput);

      // Split into directory part and completion part
      auto elements = partialPath.elements();
      std::string toComplete;

      // Handle completion within a directory if path ends with /
      bool endsWithSlash = !partialInput.empty() && partialInput.back() == '/';

      if (!elements.empty() && !endsWithSlash)
      {
        toComplete = elements.back();
        elements.pop_back();
      }

      // Find the directory to complete in
      const Directory* targetDir = &currentDir;

      if (!elements.empty())
      {
        Path dirPath(elements, partialPath.isAbsolute());
        PathResolver resolver(const_cast<Directory&>(currentDir));
        auto* node = resolver.resolve(dirPath, currentDir);

        if (!node || !node->isDirectory()) {
          return CompletionResult{};
        }

        if (node->getAccessLevel() > accessLevel) {
          return CompletionResult{};
        }

        targetDir = static_cast<const Directory*>(node);
      }

      // Get completions in the target directory
      return completeInDirectory(*targetDir, toComplete, elements, accessLevel, partialPath.isAbsolute());
    }

  private:
    static CompletionResult listCurrentDirectory(const Directory& dir, AccessLevel accessLevel)
    {
      CompletionResult result;

      dir.traverse(
        [&result, accessLevel](const NodeIf& node, size_t depth)
        {
          if (depth == 1 && node.getAccessLevel() <= accessLevel) {
            result.allOptions.push_back(node.getName() + (node.isDirectory() ? "/" : ""));
          }
        },
        0
      );

      std::sort(result.allOptions.begin(), result.allOptions.end());
      return result;
    }

    static CompletionResult completeInDirectory(
      const Directory& dir, 
      const std::string& partial,
      const std::vector<std::string>& pathElements,
      AccessLevel accessLevel,
      bool isAbsolute)
    {
      CompletionResult result;

      // First pass: collect all matching entries
      dir.traverse(
        [&](const NodeIf& node, size_t depth) {
          if (depth == 1 && node.getAccessLevel() <= accessLevel)
          {
            const std::string& name = node.getName();
            if (name.length() >= partial.length() && name.compare(0, partial.length(), partial) == 0)
            {
              // Store full name with directory indicator
              result.allOptions.push_back(name + (node.isDirectory() ? "/" : ""));
            }
          }
        },
        0
      );

      // Then process the collected options
      if (!result.allOptions.empty())
      {
        // Collect raw names without directory indicators for prefix finding
        std::vector<std::string> rawNames;

        for (const auto& opt : result.allOptions)
        {
          // Remove trailing "/" if it exists
          if (opt.back() == '/') {
            rawNames.push_back(opt.substr(0, opt.length() - 1));
          }
          else {
            rawNames.push_back(opt);
          }
        }

        // Find common prefix among raw names
        result.matchedNode = findCommonPrefix(rawNames);

        // Calculate new characters to be printed
        if (result.matchedNode.length() > partial.length()) {
          result.fillCharacters = result.matchedNode.substr(partial.length());
        }

        // Set directory flag based on first match
        result.isDirectory = result.allOptions[0].back() == '/';
        
        // Update full completion with common prefix
        if (!pathElements.empty())
        {
          std::string fullPath;

          if (isAbsolute) {
            fullPath = "/";
          }

          for (const auto& element : pathElements) {
            fullPath += element + "/";
          }

          result.fullPath = fullPath + result.matchedNode;
        }
        else {
          result.fullPath = (isAbsolute ? "/" : "") + result.matchedNode;
        }
      }

      return result;
    }

    static std::string findCommonPrefix(const std::vector<std::string>& strings)
    {
      if (strings.empty()) { return ""; }

      const std::string& first = strings[0];
      size_t prefixLen = first.length();

      for (size_t i = 1; i < strings.size(); ++i)
      {
        const std::string& current = strings[i];
        prefixLen = std::min(prefixLen, current.length());

        for (size_t j = 0; j < prefixLen; ++j)
        {
          if (current[j] != first[j])
          {
            prefixLen = j;
            break;
          }
        }
      }

      return first.substr(0, prefixLen);
    }
  };

}
