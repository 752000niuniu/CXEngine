# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(SimpleServer)

find_package(CXBase)
find_package(SOIL)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(IMGui)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_HEADER_PUB})


add_executable(SimpleServer 
  ${PRIVATE_HEADERS}
  src/actor/action.cpp
  src/actor/actor.cpp
  src/actor/player.cpp
  src/actor/player_state.cpp
  src/animation/frame_animation.cpp
  src/astar/astar.cpp
  src/combat/combat.cpp
  src/combat/skill.cpp
  src/graphics/shader.cpp
  src/graphics/texture.cpp
  src/graphics/bitmap.cpp
  src/entity.cpp
  src/file_system.cpp
  src/game.cpp
  src/logger.cpp
  src/message.cpp
  src/file_loading.cpp
  src/profile.cpp
  src/main.cpp
  src/resource_manager.cpp
  src/scene/base_scene.cpp
  src/scene/game_map.cpp
  src/scene/scene.cpp
  src/scene/scene_manager.cpp
  src/script_system.cpp
  src/state.cpp
  src/server.cpp
  src/time/time.cpp
  src/time/time_manager.cpp
  src/time/timer.cpp
  src/time/timer_manager.cpp
  src/window.cpp
  src/imgui/imgui_impl_glfw.cpp
  src/imgui/imgui_impl_opengl3.cpp
  src/imgui/cximgui.cpp
)

if(MSVC)
  target_compile_options(SimpleServer PRIVATE /wd4324 /wd4201 /wd4819  )
else()
  # target_compile_options(SimpleServer PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(SimpleServer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        # ${CMAKE_CURRENT_SOURCE_DIR}/..
        # ${CMAKE_CURRENT_SOURCE_DIR}/../common
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/dist/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/dist/include/freetype2
)
target_compile_definitions(SimpleServer 
  PRIVATE 
    -DGLEW_STATIC
    -DIMGUI_IMPL_OPENGL_LOADER_GLEW
    -DWORK_DIR="${ENGINE_DIR}/"
    -DSIMPLE_SERVER 
)

set_target_properties(SimpleServer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(SimpleServer)
target_link_libraries(SimpleServer
PRIVATE
  glfw
  GLEW
  SOIL
  CXBase::CXBase
  IMGui::IMGui
)