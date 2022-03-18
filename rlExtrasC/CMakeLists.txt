cmake_minimum_required (VERSION 3.12)

project("rlExtrasC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -march=native")

file(GLOB RL_EXTRAS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_library(${PROJECT_NAME} SHARED
    ${RL_EXTRAS_SOURCES}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} raylib)