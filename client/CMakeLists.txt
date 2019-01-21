cmake_minimum_required (VERSION 3.10)
project (SimpleEngine)

find_package(luadbg)
find_package(CXBase)
find_package(SOIL)
find_package(GLEW CONFIG)
find_package(glfw3)
find_package(freetype CONFIG)
find_package(IMGui)

file(GLOB_RECURSE  SOURCES_CPP "src/*.cpp")
file(GLOB_RECURSE  SOURCES_HEADER_PRI "src/*.h" )
file(GLOB_RECURSE  PRIVATE_HEADERS "src/*.h" "src/*.hpp" "src/*.inl")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files"  FILES ${SOURCES_CPP})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Header Files"  FILES ${SOURCES_HEADER_PRI})

add_executable(SimpleEngine
    ${PRIVATE_HEADERS}
    src/actor/action.cpp
    src/actor/Actor.cpp
    src/actor/ActorManager.cpp
    src/actor/Player.cpp
    src/actor/PlayerState.cpp
    src/algorithm.cpp
    src/animation/FrameAnimation.cpp
    src/astar/Astar.cpp
    src/Bitmap.cpp
    src/camera.cpp
    src/combat/Combat.cpp
    src/combat/Skill.cpp
    src/Components/BaseComponent.cpp
    src/Components/TransformComponent.cpp
    src/core/Graphics2D.cpp
    src/core/Object2D.cpp
    src/core/Renderer.cpp
    src/core/Shader.cpp
    src/core/Texture.cpp
    src/core/Transform.cpp
    src/CubeBatch.cpp
    #src/debugger.cpp
    src/demo_scene.cpp
    src/editor/resource_viewer.cpp
    src/Engine.cpp
    src/Entity.cpp
    src/file_system.cpp
    src/Game.cpp
    src/ImageLoader.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
    src/InputManager.cpp
    src/Logger.cpp
    #src/material.cpp
    #src/mesh.cpp
    src/Message.cpp
    #src/model.cpp
    src/net.cpp
    src/ne_wrapper.cpp
    src/Objects/MapObject.cpp
    src/Objects/Object.cpp
    src/profile.cpp
    src/main.cpp
    src/Resource.cpp
    src/ResourceManager.cpp
    src/scene/BaseScene.cpp
    src/scene/GameMap.cpp
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    src/scene/SplashScene.cpp
    src/scene/UIScene.cpp
    src/scene/WASViewerScene.cpp
    src/script_system.cpp
    src/SpriteBatch.cpp
    src/SpriteRenderer.cpp
    src/State.cpp
    src/test_scene.cpp
    src/TextRenderer.cpp
    src/TextureManager.cpp
    src/Time/Time.cpp
    src/Time/TimeManager.cpp
    src/Time/Timer.cpp
    src/Time/TimerManager.cpp
    src/tiny_thread_pool.cpp
    src/ui.cpp
    src/vertex.cpp
    src/Window.cpp
)

if(MSVC)
    target_compile_options(SimpleEngine PRIVATE /wd4819 )
#   target_compile_options(SimpleEngine PRIVATE /W4 /WX /wd4324 /wd4201 /wd4819 /wd4267)
else()
  target_compile_options(SimpleEngine PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

target_include_directories(SimpleEngine
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/dist/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/dist/include/freetype2
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
)
if(WIN32)
target_compile_definitions(SimpleEngine 
  PRIVATE
    -DZLIB_WINAPI
    -DNOMINMAX
    -DUNICODE
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
    SOIL
    freetype
    CXBase::CXBase
    IMGui::IMGui
    luadbg
)

# if (BZIP2_FOUND)
#   include_directories(${BZIP2_INCLUDE_DIR}) 
#   list(APPEND LIBS ${BZIP2_LIBRARIES} ) 
# endif ()

# if (PNG_FOUND)
#   add_definitions(${PNG_DEFINITIONS})
#   include_directories(${PNG_INCLUDE_DIRS})
#   list(APPEND LIBS ${PNG_LIBRARIES}) 
# endif ()

# if (GZIP_FOUND)
#   add_definitions(${GZIP_DEFINITIONS})
#   include_directories(${GZIP_INCLUDE_DIRS})
#   list(APPEND LIBS ${GZIP_LIBRARIES}) 
# endif ()

# if(APPLE)
#   find_package(BZip2 QUIET REQUIRED)
#   find_package(PNG QUIET REQUIRED)
# endif(APPLE)

# if( MSVC )
#   # in order to prevent DLL hell, each of the DLLs have to be suffixed with the major version and msvc prefix
#   if( MSVC70 OR MSVC71 )
#     set(MSVC_PREFIX "vc70")
#   elseif( MSVC80 )
#     set(MSVC_PREFIX "vc80")
#   elseif( MSVC90 )
#     set(MSVC_PREFIX "vc90")
#   elseif( MSVC10 )
#     set(MSVC_PREFIX "vc100")
#   elseif( MSVC11 )
#     set(MSVC_PREFIX "vc110")
#   elseif( MSVC12 )
#     set(MSVC_PREFIX "vc120")
#   elseif( MSVC14 )
#     set(MSVC_PREFIX "vc140")
#   else()
#     set(MSVC_PREFIX "vc150")
#   endif()
#   set(ASSIMP_LIB "assimp-${MSVC_PREFIX}-mt" )
# else()
#   set(ASSIMP_LIB assimp)
# endif()





# if(WIN32)
#   find_package(OpenGL REQUIRED)
#   list(APPEND LIBS opengl32) 
#   list(APPEND LIBS ${COMMON_LIBS}) 
#   list(APPEND LIBS ${OPENGL_LIBRARY}) 
#   list(APPEND LIBS ${ASSIMP_LIB}) 
#   list(APPEND LIBS IrrXML) 
#   list(APPEND LIBS zlibstaticd) 
# elseif(UNIX)
#   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
#   find_package(OpenGL REQUIRED)
#   list(APPEND LIBS ${OPENGL_gl_LIBRARY})
#   list(APPEND LIBS ${COMMON_LIBS}) 
#   add_definitions(${OPENGL_DEFINITIONS})
#   find_package(X11 REQUIRED)
#   list(APPEND LIBS ${X11_Xrandr_LIB} ${X11_Xxf86vm_LIB} ${X11_Xinerama_LIB} ${X11_Xi_LIB} ${X11_Xcursor_LIB})
#   find_package(Threads REQUIRED)
#   list(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
# else()
#   set(LIBS )
# endif(WIN32)

# # Because we use glfw3 we need to link the application with Cocoa
# IF(APPLE)
#   INCLUDE_DIRECTORIES(/System/Library/Frameworks)
#   FIND_LIBRARY(Cocoa_LIBRARY Cocoa)
#   FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
#   FIND_LIBRARY(IOKit_LIBRARY IOKit)
#   FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
#   MARK_AS_ADVANCED(Cocoa_LIBRARY OpenGL_LIBRARY)
#   SET(APPLE_LIBS ${Cocoa_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
#   set(LIBS ${LIBS} ${APPLE_LIBS})
# ENDIF(APPLE)






#set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Xcode and Visual working directories

#create_target_launcher(${PROJECT_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/")
#create_default_target_launcher(${PROJECT_NAME} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/")

#add_subdirectory(src)


