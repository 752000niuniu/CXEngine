#pragma once
#include <stdint.h>

typedef int64_t 	int64;
typedef int32_t 	int32;
typedef int16_t 	int16;
typedef int8_t 		int8;

typedef uint64_t	uint64;
typedef uint32_t 	uint32;
typedef uint16_t 	uint16;
typedef uint8_t 	uint8;

typedef float		float32;
typedef double		float64;
typedef long double float128;

typedef uint8_t   byte;

#include <cstdlib>
#include <string>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>

#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include <functional>
#include <chrono>

#include <vector>
#include <array>
#include <list>
#include <map>
#include <deque>
#include <set>
#include <random>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <nlohmann/json.hpp>
#include <NESupport.h>
#include "lua_bind.h"
#include "Singleton.h"	
#include "Logger.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#endif
#include <imgui.h>

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>



#ifndef TGA_FILE_HEADER_H
#define TGA_FILE_HEADER_H
#pragma pack(push) 
#pragma pack(1)   
struct TGA_FILE_HEADER
{
	uint8 IdLength;
	uint8 ColorMapType;
	uint8 ImageType;
	uint16 ColorMapFirstIndex;
	uint16 ColorMapLength;
	uint8 ColorMapEntrySize;
	uint16 XOrigin;
	uint16 YOrigin;
	uint16 ImageWidth;
	uint16 ImageHeight;
	uint8 PixelDepth;
	uint8 ImageDescruptor;
};
#pragma pack(pop)
#endif

#ifdef _T
	#undef _T
#endif

#define _T(x) std::string(x)
