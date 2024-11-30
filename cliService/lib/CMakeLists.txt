set(SOURCES
  src/actionRequest/ActionRequest.cpp
  src/tree/CommandIf.cpp
  src/tree/Directory.cpp
  src/tree/NodeIf.cpp
)

add_library(${PROJECT_NAME}_lib
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
