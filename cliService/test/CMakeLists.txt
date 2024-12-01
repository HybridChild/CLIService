# ActionRequest tests
add_executable(${PROJECT_NAME}_ActionRequest_test
  tests/parser/requests/ActionRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_ActionRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# LoginRequest tests
add_executable(${PROJECT_NAME}_LoginRequest_test
  tests/parser/requests/LoginRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_LoginRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)


# InputHandler tests
add_executable(${PROJECT_NAME}_InputHandler_test
  tests/parser/InputHandlerTest.cpp
  mock/terminal/TerminalMock.cpp
)

target_include_directories(${PROJECT_NAME}_InputHandler_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_InputHandler_test
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

gtest_discover_tests(${PROJECT_NAME}_ActionRequest_test)
gtest_discover_tests(${PROJECT_NAME}_LoginRequest_test)
gtest_discover_tests(${PROJECT_NAME}_InputHandler_test)
gtest_discover_tests(${PROJECT_NAME}_Tree_test)
