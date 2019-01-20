cmake_minimum_required ( VERSION 3.5 )
project ( internals  )
#set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/dist )

add_subdirectory(nlohmann)
add_subdirectory(kbase-cmake)
add_subdirectory(ezio-cmake)
add_subdirectory(lua-cmake)
add_subdirectory(luacjson-cmake)
add_subdirectory(NESupport)
add_subdirectory(imgui)
add_subdirectory(cxlua)
add_subdirectory(cxbase)

