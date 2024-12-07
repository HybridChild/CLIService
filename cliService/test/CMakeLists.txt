# ActionRequest tests
add_executable(${PROJECT_NAME}_ActionRequest_test
  tests/requests/ActionRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_ActionRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# LoginRequest tests
add_executable(${PROJECT_NAME}_LoginRequest_test
  tests/requests/LoginRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_LoginRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# InputParser tests
add_executable(${PROJECT_NAME}_InputParser_test
  tests/parser/InputParserTest.cpp
  mock/terminal/TerminalMock.cpp
)

target_include_directories(${PROJECT_NAME}_InputParser_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_InputParser_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# Tree tests
add_executable(${PROJECT_NAME}_Tree_test
  tests/tree/TreeTest.cpp
)

target_link_libraries(${PROJECT_NAME}_Tree_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# CLIService tests
add_executable(${PROJECT_NAME}_CLIService_test
  tests/cli/CLIServiceTest.cpp
  mock/terminal/TerminalMock.cpp
  mock/tree/CommandMock.hpp
)

target_include_directories(${PROJECT_NAME}_CLIService_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_CLIService_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
    GTest::gmock
)


# Path tests
add_executable(${PROJECT_NAME}_Path_test
  tests/path/PathTest.cpp
)

target_link_libraries(${PROJECT_NAME}_Path_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# PathResolver tests
add_executable(${PROJECT_NAME}_PathResolver_test
  tests/path/PathResolverTest.cpp
)

target_link_libraries(${PROJECT_NAME}_PathResolver_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# Discover tests
gtest_discover_tests(${PROJECT_NAME}_ActionRequest_test)
gtest_discover_tests(${PROJECT_NAME}_LoginRequest_test)
gtest_discover_tests(${PROJECT_NAME}_InputParser_test)
gtest_discover_tests(${PROJECT_NAME}_Tree_test)
gtest_discover_tests(${PROJECT_NAME}_CLIService_test)
gtest_discover_tests(${PROJECT_NAME}_Path_test)
gtest_discover_tests(${PROJECT_NAME}_PathResolver_test)
