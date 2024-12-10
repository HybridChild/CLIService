#include "cliService/cli/CommandHistory.hpp"
#include <gtest/gtest.h>

using namespace cliService;

class CommandHistoryTest : public ::testing::Test
{
protected:
  void SetUp() override {
    // Using smaller history size for testing
    _history = std::make_unique<CommandHistory>(3);
  }

  std::unique_ptr<CommandHistory> _history;
};

TEST_F(CommandHistoryTest, EmptyHistoryReturnsEmptyString)
{
  EXPECT_TRUE(_history->getPreviousCommand().empty());
  EXPECT_TRUE(_history->getNextCommand().empty());
  EXPECT_EQ(_history->size(), 0);
}

TEST_F(CommandHistoryTest, AddSingleCommand)
{
  _history->addCommand("command1");
  
  EXPECT_EQ(_history->size(), 1);
  EXPECT_EQ(_history->getPreviousCommand(), "command1");
}

TEST_F(CommandHistoryTest, IgnoreEmptyCommands)
{
  _history->addCommand("");
  EXPECT_EQ(_history->size(), 0);
}

TEST_F(CommandHistoryTest, IgnoreDuplicateCommands)
{
  _history->addCommand("command1");
  _history->addCommand("command1");
  EXPECT_EQ(_history->size(), 1);
}

TEST_F(CommandHistoryTest, RespectMaxSize)
{
  _history->addCommand("command1");
  _history->addCommand("command2");
  _history->addCommand("command3");
  _history->addCommand("command4");
  
  EXPECT_EQ(_history->size(), 3);
  EXPECT_EQ(_history->getPreviousCommand(), "command4");
  EXPECT_EQ(_history->getPreviousCommand(), "command3");
  EXPECT_EQ(_history->getPreviousCommand(), "command2");
}

TEST_F(CommandHistoryTest, NavigationBoundaries)
{
  _history->addCommand("command1");
  _history->addCommand("command2");
  
  // Navigate backwards
  EXPECT_EQ(_history->getPreviousCommand(), "command2");
  EXPECT_EQ(_history->getPreviousCommand(), "command1");
  EXPECT_EQ(_history->getPreviousCommand(), "command1"); // Stays at start
  
  // Navigate forwards
  EXPECT_EQ(_history->getNextCommand(), "command2");
  EXPECT_TRUE(_history->getNextCommand().empty()); // Past end returns empty
}

TEST_F(CommandHistoryTest, ResetNavigation)
{
  _history->addCommand("command1");
  _history->addCommand("command2");
  
  EXPECT_EQ(_history->getPreviousCommand(), "command2");
  _history->resetNavigation();
  EXPECT_TRUE(_history->getNextCommand().empty());
}

TEST_F(CommandHistoryTest, ClearHistory)
{
  _history->addCommand("command1");
  _history->addCommand("command2");
  
  _history->clear();
  EXPECT_EQ(_history->size(), 0);
  EXPECT_TRUE(_history->getPreviousCommand().empty());
  EXPECT_TRUE(_history->getNextCommand().empty());
}

TEST_F(CommandHistoryTest, NavigationIndexTracking)
{
  _history->addCommand("command1");
  _history->addCommand("command2");
  
  EXPECT_EQ(_history->getCurrentIndex(), 2);
  _history->getPreviousCommand();
  EXPECT_EQ(_history->getCurrentIndex(), 1);
  _history->getNextCommand();
  EXPECT_EQ(_history->getCurrentIndex(), 2);
}

TEST_F(CommandHistoryTest, DownArrowAsFirstAction)
{
  // This test verifies the fix for the segmentation fault issue
  EXPECT_TRUE(_history->getNextCommand().empty());
  EXPECT_EQ(_history->getCurrentIndex(), 0);
}
