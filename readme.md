
试玩（direct playing）

链接：https://pan.baidu.com/s/1WrAw3OaPYgGZRimI4O0YyA 
提取码：0kvn 

<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/1.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/2.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/3.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/4.jpg" width="50%" height="50%">


游戏内集成玩家召唤兽属性编辑，加点修炼设置。

玩家武器 人物形象替换

玩家染色，宠物染色

玩家门派设置

原版百分之60以上技能动画

伤害结算公式 师门技能公式 玩家修炼公式  


### Notice

this repo is not suitable for you to commit now. also, not so friendly for you to run. some fundamental works haven't done yet. thanks for you watching this project. when I am done those works, let's make a complete 2D turn-based game together!!!

this repo is developed in master branch now and stable-0.1 is the stable branch.

### 对于游戏开发感兴趣的同胞们

QQ群: 983630090

### Current Working on

1. battle system
2. prepare to publish

### Introduction

this project is intend to build a relative complete 2D game. using only C++ and OpenGL,fully source code built.

this project is using CMake as build system. I am only working on Windows. 
this repo has 2 main cmake projects. 
one is SimpleEngineAll, the second one is externals. 
externals is used to build third party libs which are assumed immutable.
SimpleEngineAll is our main project. its CMakeLists.txt in the repo root dir(SimpleEngine/).

### How to build

1. install git and cmake
		
		git clone --recursive https://github.com/oceancx/SimpleEngine

2. build external project. 
using cmake.exe open external/CMakeLists.txt,set the generate project folder as external/build. then always using the default setting, until Xcode project or visual studio project was generated. using the IDE open the project, and build INSTALL.(which will automaticly copy libs and include files to external/dist)

3. build SimpleEngineAll project.
using cmake.exe open SimpleEngine/CMakeLists.txt, set the generate project folder as SimpleEngine/build. always using the default setting, until Xcode project or visual studio project was generated.using the IDE open the project, and build SimpleEngineAll.SimpleEngineAll contains 3 projects, one is client, the second is server and the 3rd is debugger. once build succeed, the executables will be at the default location SimpleEngine/bin/Debug or SimpleEngine/bin/Release which depends on your build config.
	
4. download resource. [baidu yun disk](https://pan.baidu.com/s/1o7Gn0t8), unzip to SimpleEngine/res/data.

5. run SimpleServer.exe first,run SimpleEngine.exe, click sign_up, sign_in, try to play it.

### Repo Architecture
1. third party libs: freetype2, glew, glfw, glm, SOIL
2. main internal libs: luadebugger, NESupport, ezio-cmake, imgui, cxlua
3. main executables: SimpleEngine.exe, SimpleServer.exe

luadebugger is our a repo focus on debugging lua code for SimpleEngine and SimpleServer. but it also is a standalone project. luadebugger contains two main directories luadbg/ and vscode/, vscode directory is for building vscode luadebugger extension. luadbg directory is for building luadbg.lib which is used to link to the host program(like SimpleEngine.exe). 

NESupport is a lib for decode pack resoures.

ezio-cmake is an simple and clean network lib for our project, it was modified from [ezio](https://github.com/kingsamchen/ezio) which was written by [kingsamchen](https://github.com/kingsamchen).

cxlua is a lib which encapsulate luacjson, lua, and ezio, also with some extra luabindings.

### Current Game Features
1. sign up, sign in
2. multiplayer path finding
3. multiplayer chat


### Extra Resource

https://pan.baidu.com/s/1kV9qrEB


### Last

sorry about this poor readme.md, cause making this repo as soon as stable is my first priority. if you have any specify questions want to know, please feel free to issue me or send me an email. i will be grateful to hear your feedbacks.
