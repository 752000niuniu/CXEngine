cmake_minimum_required (VERSION 3.10)
project (luadbg)

find_package(CXBase)


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../vscode-mock-debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(luadbg
  src/debugger.cpp
)

target_include_directories(luadbg
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../external/dist/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/..>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(luadbg PRIVATE -DWORKSPACE_ROOT="${PROJECT_SOURCE_DIR}")
target_link_libraries(luadbg
  PUBLIC CXBase::CXBase Nlohmann
)