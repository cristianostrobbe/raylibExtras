cmake_minimum_required (VERSION 3.12)

project("rlTilesCPP")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -W -Wno-deprecated-declarations -O0 -g -march=native")

file(GLOB RLTILES_EXTRAS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RLTiles/*.cpp)
file(GLOB PUGIXML_EXTRAS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RLTiles/PUGIXML/*.cpp)

add_library(${PROJECT_NAME} SHARED
    ${RLTILES_EXTRAS_SOURCES}
    ${PUGIXML_EXTRAS_SOURCES}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/RLTiles
    ${CMAKE_CURRENT_SOURCE_DIR}/RLTiles/PUGIXML
)

target_link_libraries(${PROJECT_NAME} raylib)

if(BUILD_ALL_EXAMPLES)
    add_executable (RLTilesTest ${CMAKE_CURRENT_SOURCE_DIR}/RLTilesTest/RLTilesTest.cpp)
    target_link_libraries(RLTilesTest ${PROJECT_NAME})
endif()