# CMake entry point
cmake_minimum_required(VERSION 3.10)
project(SimpleServer)

find_package(CXBase)
# find_package(SOIL)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(IMGui)
find_package(luadbg CONFIG)

file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_HEADER_PUB})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../common/src PREFIX "Source Files"  FILES ${GAME_SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../common/src PREFIX "Source Files"  FILES ${GAME_SOURCES_HEADER})


add_executable(SimpleServer 
  ${PRIVATE_HEADERS}
  ${GAME_SOURCES_HEADER}
  ${GAME_COMMON_SRCS}
  src/actor/player.cpp
  src/scene/scene.cpp
  src/scene/scene_manager.cpp
  src/main.cpp
  src/server.cpp
  src/window.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src
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
  # SOIL
  CXBase::CXBase
  IMGui::IMGui
  luadbg
)