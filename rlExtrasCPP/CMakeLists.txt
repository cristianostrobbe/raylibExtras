cmake_minimum_required (VERSION 3.12)

project("rlExtrasCPP")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -W -Wno-deprecated-declarations -O0 -g -march=native")

file(GLOB RLCPP_EXTRAS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} SHARED
    ${RLCPP_EXTRAS_SOURCES}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} raylib)