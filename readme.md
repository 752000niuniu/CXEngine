# 关于这个项目：

原罪西游（YZXY）是一款MMORPG类型的2D回合制开源游戏，这个项目包含了服务端和客户端以及相关工具的所有源代码。

这个项目支持读写梦幻西游经典版的资源（2.0.186以前），但是这个项目本身并不提供任何梦幻西游经典版的资源。另外本项目仅供个人学习用途，不得用于商业用途。

如果要玩原罪西游，请确保你的磁盘上有2.0.186以前的梦幻西游客户端，或者相关资源文件。

### 已构建完成的库下载

https://pan.baidu.com/s/18S0OKkdUJIRutJ5MuaLEug 提取码：3ppy  

下载完成后，运行launcher.bat可以启动客户端，服务端，以及资源路径配置。

### 构建方法：

1. git clone --recursive https://github.com/oceancx/YZXY

2. 构建external工程。用cmake.exe打开YZXY/external/CMakeLists.txt，配置构建的binaries目录为YZXY/external/build, vs版本请选择Visual Studio 15 2017 之后的64位版本，之后全部采用默认配置，点击生成，直到vs工程生成。然后用IDE打开工程，构建INSTALL目标，这个构建完成后会自动安装libs还有include文件到YZXY/external/dist目录下

3. 构建SimpleEngineAll工程。用cmake.exe打开YZXY/CMakeLists.txt，配置构建的binaries目录为YZXY/build。 vs版本请选择Visual Studio 15 2017 之后的64位版本，之后全部采用默认配置，点击生成，直到vs工程生成。然后用IDE打开工程，构建SimpleEngineAll目标。成功构建完成后，YZXY/bin/Debug或者YZXY/bin/Release下会生成SimpleEngine.exe, SimpleServer.exe, cxlua.exe。

4. sh launcher.sh。即可打开launcher，从而启动服务端客户端或者配置资源目录。


### 源码介绍

externals目录下的库全是作者引用的外部库，外部的意思是作者并不打算对其做任何修改。每个引用的外部库都有自己的介绍。这里不再赘述。

internals目录下的库全是作者引用的内部库，意味着作者随时有可能对其进行修改。下面对内部库作一一介绍。

1. kbase-cmake和ezio-cmake，这两个库修改自https://github.com/kingsamchen/KBase 和 https://github.com/kingsamchen/ezio  其中ezio依赖kbase，ezio是一个小巧高效的异步网络库，c++14编写。

2. lua-cmake，luacjson-cmake，为标准的lua库的cmake版。

3. nlohmann c++json库，单.hpp文件包含

4. NESupport 梦幻西游资源读取库，单一.h&.cpp文件包含

5. cxlua 依赖lua,luacjson,ezio,nesupport,nlohmann,glm。cxlua包装了基础性的c++接口，让其可以被lua调用到。SimpleEngine，SimpleServer以及launcher都依赖cxlua。

6. luadebugger https://github.com/oceancx/luadebugger  这个目录是一个repo，提供了lua调试器，包含了vscode的lua调试器适配器，以及嵌入到被调试程序里的调试相关功能的支持（例如断点，步入，步出）

7. gsm-cmake，miniaudio-cmake 修改自https://github.com/dr-soft/miniaudio 使其支持播放相关音频文件。

8. imgui https://github.com/ocornut/imgui ，游戏必备，本项目也提供了imgui的lua binding生成。

9. nanovg 2D矢量图形库 https://github.com/memononen/nanovg

下面是YZXY/下剩余目录的介绍

1. .vscode保存的是项目的lua调试器配置，关于本项目调试器的构建安装请看launcher里的说明
2. bin/目录 是默认的exe构建生成目录
3. common/目录下的.cpp/.h文件会同时参与SimpleEngine和SimpleServer的构建。
4. client/仅用于SimpleEngine的cpp/h
5. server/仅用于SimpleServer的cpp/h
6. data/梦幻西游资源存放目录
7. cxluax是launcher项目
8. res/目录下是一些其他资源，例如字体，图标，tsv行列表
9. scripts/是YZXY用到的所有lua文件，scripts/share/目录下的lua文件共享于launcher/SimpleEngine/SimpleServer，scripts/client/仅SimpleEngine使用，scripts/server仅SimpleServer使用


SimpleEngine，SimpleServer，以及launcher主要逻辑都由lua实现，其lua入口为各自的main.lua文件。

enums_protocol.lua 定义了所有客户端服务端之间的协议


### 原罪西游介绍

<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/1.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/2.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/3.jpg" width="50%" height="50%">
<img src="https://github.com/oceancx/SimpleEngine/blob/master/res/icon/4.jpg" width="50%" height="50%">


ALT+E 可以给自己染色，更换形象，更换武器

ALT+W 可以编辑场景里玩家以及召唤兽的属性，包括玩家属性点，玩家级别，玩家技能等级，玩家门派，玩家修炼，召唤兽属性点，召唤兽等级，召唤兽修炼，玩家携带的召唤兽信息，以及玩家的队伍信息。

ALT+R 仅战斗中会显示/隐藏，可以编辑当前战斗中所有对象的战斗指令。攻击/法术/防御/逃跑等


原罪西游会朝着尽可能还原梦幻西游经典版的方向开发，目前技术方面已经无压力，后续功能基本上用lua便可实现。目前正在完善战斗部分，已经有了基础的战斗控制编辑器，以及actor属性编辑器。

欢迎对此项目感兴趣的朋友加Q群讨论：983630090


