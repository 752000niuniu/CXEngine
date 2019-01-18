# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(game_server)

find_package(CXBase)
find_package(NESupport)
find_package(glm)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

add_executable(game_server 
  ${PRIVATE_HEADERS}
  src/actor/player.cpp
  src/astar/Astar.cpp
  #src/debugger.cpp
  src/file_system.cpp
  src/Game.cpp
  src/Logger.cpp
  src/main.cpp
  src/scene/BaseScene.cpp
  src/scene/GameMap.cpp
  src/scene/Scene.cpp
  src/scene/SceneManager.cpp
  src/script_system.cpp
  src/server.cpp
)

if(MSVC)
  target_compile_options(cxbase PRIVATE /W4 /WX /wd4324 /wd4201 /wd4819  )
else()
  target_compile_options(cxbase PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(game_server
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_options(game_server PRIVATE /wd4819)
target_compile_definitions(game_server PRIVATE -DWORK_DIR="${ENGINE_DIR}/")
target_link_libraries(game_server
  CXBase::CXBase
  NESupport::NESupport
  glm
)