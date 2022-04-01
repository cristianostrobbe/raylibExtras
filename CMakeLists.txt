cmake_minimum_required (VERSION 3.12)

project("raylibExtras" VERSION 1.1)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -W -Wno-deprecated-declarations -Wno-missing-field-initializers -O0 -g -march=native")

option(DEBUG_MODE "More debug information" ON)
if (DEBUG_MODE)
    add_definitions(-DEBUG_MODE)
endif()
option(BUILD_ALL_EXAMPLES "Buils all libraries examples" ON)

find_package(glfw3 3.3 REQUIRED)

add_subdirectory(raylib)
# add_subdirectory(rlExtrasC)
add_subdirectory(rlExtrasCPP)
add_subdirectory(rlTilesCPP)

set(IMGUI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui CACHE PATH "ImGui source directory")

file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS ${IMGUI_SOURCE_DIR}/*.cpp)
file(GLOB RL_IMGUI_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui/*.cpp)

add_library(${PROJECT_NAME} SHARED
    ${IMGUI_SOURCES}
    ${RL_IMGUI_SOURCES}
)

include_directories(
    ${IMGUI_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui
    # ${CMAKE_CURRENT_SOURCE_DIR}/rlExtrasC
    ${CMAKE_CURRENT_SOURCE_DIR}/rlExtrasCPP
)

target_link_libraries(${PROJECT_NAME} glfw raylib rlExtrasCPP)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

if(BUILD_ALL_EXAMPLES)
    add_executable (FPCameraCPPSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FPCameraCPPSample/example.cpp)
    target_link_libraries(FPCameraCPPSample ${PROJECT_NAME})

    add_executable (FollowCameraCPPSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FollowCameraCPPSample/example.cpp)
    target_link_libraries(FollowCameraCPPSample ${PROJECT_NAME})

    add_executable (ImGuiSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/ImGuiSample/example.cpp)
    target_link_libraries(ImGuiSample ${PROJECT_NAME})

    add_executable (ImGuiRaylib ${CMAKE_CURRENT_SOURCE_DIR}/Examples/ImGuiRaylib/example.cpp)
    target_link_libraries(ImGuiRaylib ${PROJECT_NAME})
endif()