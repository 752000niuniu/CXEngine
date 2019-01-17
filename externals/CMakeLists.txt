cmake_minimum_required ( VERSION 3.5 )
project ( externals  )
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/dist )

list(INSERT CMAKE_PREFIX_PATH 0  ${PROJECT_SOURCE_DIR})

add_subdirectory(freetype)
add_subdirectory(glew-cmake)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(SOIL)

# adding assimp
#set(ASSIMP_INSTALL_PDB FALSE)
#set(ASSIMP_BUILD_TESTS FALSE)
#set(ASSIMP_BUILD_ASSIMP_TOOLS FALSE)
#set(BUILD_SHARED_LIBS FALSE)
#set(LINK_SEARCH_START_STATIC TRUE)
#add_subdirectory(assimp)