# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(GameServer)

find_package(CXBase)
find_package(NESupport)
find_package(glm)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
add_executable(gameserver 
  ${PRIVATE_HEADERS}
  src/actor/player.cpp
  src/astar/Astar.cpp
  #src/debugger.cpp
  src/file_system.cpp
  src/Game.cpp
  src/GMath.cpp
  src/kbase/error_exception_util.cpp
  src/kbase/string_encoding_conversions.cpp
  src/kbase/string_format.cpp
  src/kbase/string_util.cpp
  src/Logger.cpp
  src/main.cpp
  src/Random.cpp
  src/scene/BaseScene.cpp
  src/scene/GameMap.cpp
  src/scene/Scene.cpp
  src/scene/SceneManager.cpp
  src/script_system.cpp
  src/server.cpp
  src/tsv.cpp
)
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})

file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

target_include_directories(gameserver
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_options(gameserver PRIVATE /wd4819)
target_compile_definitions(gameserver PRIVATE -DWORK_DIR="${ENGINE_DIR}/")
target_link_libraries(gameserver
  CXBase::CXBase
  NESupport::NESupport
  glm
)