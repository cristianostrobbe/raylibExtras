cmake_minimum_required (VERSION 3.12)

project("raylibExtras")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -W -Wno-deprecated-declarations -Wno-missing-field-initializers -O0 -g -march=native")

option(DEBUG_MODE "More debug information" ON)
if (DEBUG_MODE)
    add_definitions(-DEBUG_MODE)
endif()
option(BUILD_ALL_EXAMPLES "Buils all libraries examples" OFF)

find_package(glfw3 3.2 REQUIRED)
find_package(nlohmann_json REQUIRED)

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

target_link_libraries(${PROJECT_NAME} glfw raylib rlExtrasCPP nlohmann_json)

add_executable (FPCameraCPPSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FPCameraCPPSample/example.cpp)
target_link_libraries(FPCameraCPPSample ${PROJECT_NAME})

add_executable (FollowCameraCPPSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FollowCameraCPPSample/example.cpp)
target_link_libraries(FollowCameraCPPSample ${PROJECT_NAME})

add_executable (TestFollowCameraCPPSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FollowCameraCPPSample/test.cpp)
target_link_libraries(TestFollowCameraCPPSample ${PROJECT_NAME})

add_executable (TestVehicleController ${CMAKE_CURRENT_SOURCE_DIR}/Examples/FollowCameraCPPSample/test_vehicle_controller.cpp)
target_link_libraries(TestVehicleController ${PROJECT_NAME})

add_executable (ImGuiSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/ImGuiSample/example.cpp)
target_link_libraries(ImGuiSample ${PROJECT_NAME})

add_executable (ImGuiRaylib ${CMAKE_CURRENT_SOURCE_DIR}/Examples/ImGuiRaylib/example.cpp)
target_link_libraries(ImGuiRaylib ${PROJECT_NAME})

# add_executable (map_traj_imgui ${CMAKE_CURRENT_SOURCE_DIR}/Examples/map_traj_imgui.cpp)
# target_link_libraries(map_traj_imgui ${PROJECT_NAME})

# add_executable (RLSpritesCppSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/RLSpritesCppSample/simple_flip_example.cpp)
# target_link_libraries(RLSpritesCppSample ${PROJECT_NAME})

# add_executable (TPOrbitCameraCSample ${CMAKE_CURRENT_SOURCE_DIR}/Examples/TPOrbitCameraCSample/example.c)
# target_link_libraries(TPOrbitCameraCSample ${PROJECT_NAME})