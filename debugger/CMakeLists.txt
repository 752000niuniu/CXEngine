# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (debugger)

find_package(CXBase)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../vscode-mock-debug)

add_executable(debugger
  src/main.cpp
  src/debugger_adapter.cpp
  src/script_system.cpp
)
file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})

file(GLOB_RECURSE  SOURCES_HEADER_PUB "src/*.h" )
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_PUB})

target_include_directories(debugger
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../external/dist/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/..>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(debugger PRIVATE -DWORKSPACE_ROOT="${PROJECT_SOURCE_DIR}")
target_link_libraries(debugger
  PUBLIC CXBase::CXBase
)