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