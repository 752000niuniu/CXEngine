# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (vsdbgadpter)

find_package(CXLua)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h"  "src/*.inl")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})

add_executable(vsdbgadpter
  ${SOURCES_HEADER_RPI}
  src/debugger_adapter.cpp
)

target_include_directories(vsdbgadpter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vsdbgadpter
  PUBLIC CXLua::CXLua
)

configure_file(main.lua  ${CMAKE_SOURCE_DIR}/bin/Debug/main.lua COPYONLY)
configure_file(threads.lua     ${CMAKE_SOURCE_DIR}/bin/Debug/threads.lua COPYONLY)