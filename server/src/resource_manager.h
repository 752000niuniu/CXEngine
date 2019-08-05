/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#pragma once


#include "graphics/texture.h"
#include "script_system.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.

#define AddonWDF      0x10
#define AtomWDF       0x20
#define ChatWDF       0x30
#define FireworkWDF   0x40
#define GoodsWDF      0x50
#define ItemWDF       0x60
#define MagicWDF      0x70
#define MapaniWDF     0x80
#define MhimageWDF    0x90
#define MiscWDF       0xa0
#define MusicWDF      0xb0
#define SceneWDF      0xc0
#define ShapeWDF      0xd0
#define ShapeWD1      0xd1
#define ShapeWD2      0xd2
#define ShapeWD3      0xd3
#define ShapeWD4      0xd4
#define ShapeWD5      0xd5
#define ShapeWD6      0xd6
#define ShapeWD7      0xd7
#define SmapWDF       0xe0
#define SoundWDF      0xf0
#define StockWDF      0x100
#define WaddonWDF     0x110
#define WzifeWDF      0x120
#define WzifeWD1      0x121

namespace utils
{
	static std::string GetPathByPackID(uint32 packID);
};

class ResourceManager final : public Singleton<ResourceManager>
{
public:

	friend Singleton<ResourceManager>;

	Sprite* LoadWASSpriteByID(uint64_t resID,bool sync = false);

	void UnLoadWASSpriteByID(uint64_t resID);

	Sprite* LoadWASSprite(uint32_t pack,uint32 wasId,bool sync = false);

	void UnLoadWASSprite(uint32_t pack, uint32 wasId);
    
	void Clear();

	uint64_t EncodeWAS(uint32_t pack, uint32_t wasID) { uint64_t resID = pack;return (resID << 32) | wasID;}
	
	void DecodeWAS(uint64_t resID, uint32_t& pack, uint32_t& wasID) {  pack = resID >> 32; wasID = (resID & 4294967295); };
	
	void OnUpdate();
private:
	ResourceManager();

	~ResourceManager();

	std::map<std::string, Texture*> Textures;
};
#define  RESOURCE_MANAGER_INSTANCE ResourceManager::GetInstance()


void luaopen_resource_manager(lua_State* L);