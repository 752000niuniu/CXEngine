cmake_minimum_required (VERSION 3.10)
project (SimpleEngine LANGUAGES C CXX)

find_package(CXBase)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(freetype CONFIG)
find_package(IMGui)
find_package(luadbg_static)
find_package(gsm)
find_package(miniaudio)
find_package(nanovg)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp" )
file(GLOB_RECURSE  SOURCES_HEADER_PRI "src/*.h"  "src/*.hpp" "src/*.inl")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_HEADER_PRI})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../common/src PREFIX "Source Files"  FILES ${GAME_SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../common/src PREFIX "Source Files"  FILES ${GAME_SOURCES_HEADER})

add_executable(SimpleEngine
    ${SOURCES_HEADER_PRI}
    src/algorithm.cpp
    src/animation/frame_animation.cpp
    src/animation/sprite.cpp
    src/camera.cpp
    src/components/base_component.cpp
    src/components/transform_component.cpp
    src/graphics/graphics_2d.cpp
    src/graphics/object_2d.cpp
    src/graphics/renderer.cpp
    src/graphics/shader.cpp
    src/graphics/texture.cpp
    src/graphics/transform.cpp
    src/graphics/bitmap.cpp
    src/graphics/ui_renderer.cpp
    src/cube_batch.cpp
    src/engine.cpp
    src/game.cpp
    src/input_manager.cpp
    src/net.cpp
    src/objects/map_object.cpp
    src/objects/object.cpp
    src/main.cpp
    src/resource_manager.cpp
    src/scene/ui_scene.cpp
    src/scene/was_viewer_scene.cpp
    src/sprite_batch.cpp
    src/sprite_renderer.cpp
    src/text_renderer.cpp
    src/texture_manager.cpp
    src/tiny_thread_pool.cpp
    src/ui.cpp
    src/vertex.cpp
    src/window.cpp
    src/audio/audio_manager.cpp


    ${GAME_SOURCES_HEADER}
    ${GAME_COMMON_SRCS}
    src/actor/player.cpp
    src/scene/scene.cpp
	src/scene/scene_manager.cpp
)

if(MSVC)
    target_compile_options(SimpleEngine PRIVATE /wd4819 )
#   target_compile_options(SimpleEngine PRIVATE /W4 /WX /wd4324 /wd4201 /wd4819 /wd4267)
else()
#   target_compile_options(SimpleEngine PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(SimpleEngine
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_compile_definitions(SimpleEngine 
  PRIVATE
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
	-DIMGUI_IMPL_OPENGL_LOADER_GLEW
    -DWORK_DIR="${ENGINE_DIR}"
    -DNODEFAULTLIB
    -DSIMPLE_ENGINE 
)
if(WIN32)
target_compile_definitions(SimpleEngine 
  PRIVATE
    -DZLIB_WINAPI
    -DNOMINMAX
    -DUNICODE
    -D_UNICODE
    -DWIN32_LEAN_AND_MEAN
    -D_WIN32_WINNT=0x0501
)
endif(WIN32)

set_target_properties(SimpleEngine PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch/precompile.h")
cotire(SimpleEngine)

target_link_libraries(SimpleEngine
  PRIVATE 
    glfw
    GLEW
    freetype
    CXBase::CXBase
    IMGui::IMGui
    luadbg_static
    gsm
    miniaudio
    nanovg
)

# add_custom_command(TARGET SimpleEngine
#     PRE_LINK
#     COMMAND ${CMAKE_COMMAND} -E copy ${ENGINE_DIR}/externals/dist/bin/luadbg.dll ${ENGINE_DIR}/bin/$<CONFIG>/
# )
