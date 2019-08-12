#include "resource_manager.h"
#include "graphics/image.h"
#include "file_system.h"
#include "profile.h"
#include "file_loading.h"
#include <string>
#include "logger.h"


static std::vector<String> s_PackToName = { "addon.wdf","atom.wdf","chat.wdf","firework.wdf","goods.wdf","item.wdf","magic.wdf","mapani.wdf","mhimage.wdf","misc.wdf","music.wdf","scene.wdf","shape.wd1","shape.wd2","shape.wd3","shape.wd4","shape.wd5","shape.wd6","shape.wd7","shape.wdf","smap.wdf","sound.wdf","stock.wdf","waddon.wdf","wzife.wd1","wzife.wdf","wzimage.wdf" };
static std::map<uint32_t, NE::WDF*> s_Loaders;

ResourceManager::ResourceManager()
	:Singleton<ResourceManager>()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::Clear()
{
    
}

Sprite* ResourceManager::LoadWASSpriteByID(uint64_t resID,bool sync)
{
	if (!resID)return nullptr;
	std::string path(std::to_string(resID));
	uint32_t pack = 0; 
	uint32_t wasID = 0;
	DecodeWAS(resID, pack, wasID);

	auto* iothread = file_loading_thread();
	if (iothread->IsFileLoadOk(path.c_str()))
	{
		return s_Loaders[pack]->LoadSprite(wasID);
	}
	else
	{
		if (sync)
		{
			if (s_Loaders.find(pack) == s_Loaders.end())
			{
				s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
			}
			return s_Loaders[pack]->LoadSprite(wasID);
		}
		else
		{
			iothread->PostTask(path.c_str(), [this, pack, wasID](const char* path)->bool
			{
				if (s_Loaders.find(pack) == s_Loaders.end())
				{
					s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
				}
				s_Loaders[pack]->LoadSprite(wasID);
				return true;
			});
			return nullptr;
		}
	}
}

void ResourceManager::UnLoadWASSpriteByID(uint64_t resID)
{
	uint32_t pack; 
	uint32_t wasID;
	DecodeWAS(resID, pack, wasID);
	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		return;
	}
	else
	{
		s_Loaders[pack]->UnLoadSprite(wasID);
		return;
	}
}


Sprite* ResourceManager::LoadWASSprite(uint32_t pack, uint32 wasID,bool sync)
{
	auto resID = EncodeWAS(pack, wasID);
	return LoadWASSpriteByID(resID,sync);
}

void ResourceManager::UnLoadWASSprite(uint32_t pack, uint32 wasID)
{
	auto resID = EncodeWAS(pack, wasID);
	return UnLoadWASSpriteByID(resID);
}


void ResourceManager::OnUpdate()
{

}


static std::map<uint32_t, std::string> s_PackPathMap =
{
	{ AddonWDF,    "addon.wdf" },
{ AtomWDF,     "atom.wdf" },
{ ChatWDF,     "chat.wdf" },
{ FireworkWDF, "firework.wdf" },
{ GoodsWDF,    "goods.wdf" },
{ ItemWDF,     "item.wdf" },
{ MagicWDF,    "magic.wdf" },
{ MapaniWDF,   "mapani.wdf" },
{ MhimageWDF,  "mhimage.wdf" },
{ MiscWDF,     "misc.wdf" },
{ MusicWDF,    "music.wdf" },
{ SceneWDF,    "scene.wdf" },
{ ShapeWDF,    "shape.wdf" },
{ ShapeWD1,    "shape.wd1" },
{ ShapeWD2,    "shape.wd2" },
{ ShapeWD3,    "shape.wd3" },
{ ShapeWD4,    "shape.wd4" },
{ ShapeWD5,    "shape.wd5" },
{ ShapeWD6,    "shape.wd6" },
{ ShapeWD7,    "shape.wd7" },
{ SmapWDF,     "smap.wdf" },
{ SoundWDF,    "sound.wdf" },
{ StockWDF,    "stock.wdf" },
{ WaddonWDF,   "waddon.wdf" },
{ WzifeWDF,    "wzife.wdf" },
{ WzifeWD1,    "wzife.wd1" },
};

namespace utils
{
	std::string GetPathByPackID(uint32 packID)
	{
		auto it = s_PackPathMap.find(packID);
		if (it != s_PackPathMap.end())
		{
			return FileSystem::GetWDFPath(it->second);
		}
		else
		{
			printf("GetPathByPackID error!\n");
			return "";
		}
	}
}



void resource_manager_init()
{
	RESOURCE_MANAGER_INSTANCE;
}

void resource_manager_update()
{

	RESOURCE_MANAGER_INSTANCE->OnUpdate();

}
void resource_manager_deinit()
 {
	RESOURCE_MANAGER_INSTANCE->Clear();
}

void luaopen_resource_manager(lua_State* L)
{
#define REG_ENUM(name, macro)  (lua_pushinteger(L, macro),lua_setglobal(L, name))
	REG_ENUM("AddonWDF", AddonWDF);
	REG_ENUM("AtomWDF", AtomWDF);
	REG_ENUM("ChatWDF", ChatWDF);
	REG_ENUM("FireworkWDF", FireworkWDF);
	REG_ENUM("GoodsWDF", GoodsWDF);
	REG_ENUM("ItemWDF", ItemWDF);
	REG_ENUM("MagicWDF", MagicWDF);
	REG_ENUM("MapaniWDF", MapaniWDF);
	REG_ENUM("MhimageWDF", MhimageWDF);
	REG_ENUM("MiscWDF", MiscWDF);
	REG_ENUM("MusicWDF", MusicWDF);
	REG_ENUM("SceneWDF", SceneWDF);
	REG_ENUM("ShapeWDF", ShapeWDF);
	REG_ENUM("ShapeWD1", ShapeWD1);
	REG_ENUM("ShapeWD2", ShapeWD2);
	REG_ENUM("ShapeWD3", ShapeWD3);
	REG_ENUM("ShapeWD4", ShapeWD4);
	REG_ENUM("ShapeWD5", ShapeWD5);
	REG_ENUM("ShapeWD6", ShapeWD6);
	REG_ENUM("ShapeWD7", ShapeWD7);
	REG_ENUM("SmapWDF", SmapWDF);
	REG_ENUM("SoundWDF", SoundWDF);
	REG_ENUM("StockWDF", StockWDF);
	REG_ENUM("WaddonWDF", WaddonWDF);
	REG_ENUM("WzifeWDF", WzifeWDF);
	REG_ENUM("WzifeWD1", WzifeWD1);
#undef REG_ENUM


	script_system_register_function(L, resource_manager_init);
	script_system_register_function(L, resource_manager_update);
	script_system_register_function(L, resource_manager_deinit);
	
}
