# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(GameServer)

find_package(CXBase)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_executable(GameServer 
  ${PRIVATE_HEADERS}
  src/actor/player.cpp
  src/astar/astar.cpp
  src/file_system.cpp
  src/game.cpp
  src/logger.cpp
  src/main.cpp
  src/scene/base_scene.cpp
  src/scene/game_map.cpp
  src/scene/scene.cpp
  src/scene/scene_manager.cpp
  src/script_system.cpp
  src/server.cpp
)

if(MSVC)
  target_compile_options(GameServer PRIVATE /wd4324 /wd4201 /wd4819  )
else()
  # target_compile_options(GameServer PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(GameServer
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(GameServer PRIVATE -DWORK_DIR="${ENGINE_DIR}/")

set_target_properties(GameServer PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(GameServer)
target_link_libraries(GameServer
  CXBase::CXBase
)