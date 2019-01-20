# CMake entry point
cmake_minimum_required (VERSION 3.10)
project (vsdbgadpter)

find_package(CXLua)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../vscode-mock-debug)
#set(RUNTIME_OUTPUT_DIRECTORY_$<CONFIG:Debug> ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_RPI "src/*.h" )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_RPI})

add_executable(vsdbgadpter
  ${SOURCES_HEADER_RPI}
  src/main.cpp
  src/debugger_adapter.cpp
  #src/script_system.cpp
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

#install(DIRECTORY include/ DESTINATION include)
#install(TARGETS vsdbgadpter DESTINATION bin)
#set(xx $<TARGET_FILE_DIR:vsdbgadpter>)
#set(prop "$<TARGET_PROPERTY:INCLUDE_DIRECTORIES>")
#message($<TARGET_PROPERTY:INCLUDE_DIRECTORIES>)
configure_file(main.lua  ${CMAKE_SOURCE_DIR}/bin/Debug/main.lua COPYONLY)
configure_file(threads.lua     ${CMAKE_SOURCE_DIR}/bin/Debug/threads.lua COPYONLY)

#configure_file(threads.lua   $<TARGET_FILE_DIR:vsdbgadpter>/threads.lua COPYONLY)
