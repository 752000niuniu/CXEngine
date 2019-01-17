### Notice

this project is not suitable for you to commit now. also, not so friendly for you to run. some fundamental works haven't done yet. thanks for you watching this project. when I am done those works, let's make a complete 2D turn-based game together!!!

### 对于游戏开发感兴趣的同胞们

QQ群: 465885387

### Current Working on

1. achitect lua module
2. prepare to publish

### Introduction

this project is intend to build a relative complete 2D game. using only C++ and OpenGL,fully source code built.

this project is using CMake as build system. the author now is only work on Windows. this repo has 2 projects. one is SimpleEngineAll, the second one is externals. externals is used to build third party libs which are assumed immutable.
SimpleEngineAll is our main project. its CMakeLists.txt in the repo root dir(SimpleEngine/).

### How to build

1. install git and cmake
		
		git clone --recursive https://github.com/oceancx/SimpleEngine

2. build external project. 
using cmake.exe open external/CMakeLists.txt,set the generate project folder as external/build. then always using the default setting, until Xcode project or visual studio project was generated. using the IDE open the project, and build INSTALL.(which will automaticly copy libs and include files to external/dist)

3. build SimpleEngineAll project.
using cmake.exe open SimpleEngine/CMakeLists.txt, set the generate project folder as SimpleEngine/build. always using the default setting, until Xcode project or visual studio project was generated.using the IDE open the project, and build SimpleEngineAll.SimpleEngineAll contains 3 projects, one is client, the second is server and the 3rd is debugger. once build succeed, the executables will be at the default location SimpleEngine/bin/Debug or SimpleEngine/bin/Release which depends on your build config.
	
4. download resource. [baidu yun disk](https://pan.baidu.com/s/1o7Gn0t8), unzip to SimpleEngine/res/data.

5. run gameserver.exe first, then run SimpleEngine.exe

### Program Entrance

program.cpp -> Window.cpp -> Game.cpp -> Engine.cpp -> SceneManager.cpp -> Scene.cpp

### Extra Resource

https://pan.baidu.com/s/1kV9qrEB

