set(SOURCES
  src/ActionRequest.cpp
)

add_library(${PROJECT_NAME}_lib
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
