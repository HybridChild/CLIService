# CliRequest tests
add_executable(${PROJECT_NAME}_CliRequest_test
  CliRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_CliRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)

gtest_discover_tests(${PROJECT_NAME}_CliRequest_test)

# Template for future components
# add_executable(${PROJECT_NAME}_<component>_test
#   <Component>Test.cpp
# )
# target_link_libraries(...)
# gtest_discover_tests(...)