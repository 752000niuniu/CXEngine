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
	{ ADDONWDF,    "addon.wdf" },
{ ATOMWDF,     "atom.wdf" },
{ CHATWDF,     "chat.wdf" },
{ FIREWORKWDF, "firework.wdf" },
{ GOODSWDF,    "goods.wdf" },
{ ITEMWDF,     "item.wdf" },
{ MAGICWDF,    "magic.wdf" },
{ MAPANIWDF,   "mapani.wdf" },
{ MHIMAGEWDF,  "mhimage.wdf" },
{ MISCWDF,     "misc.wdf" },
{ MUSICWDF,    "music.wdf" },
{ SCENEWDF,    "scene.wdf" },
{ SHAPEWDF,    "shape.wdf" },
{ SHAPEWD1,    "shape.wd1" },
{ SHAPEWD2,    "shape.wd2" },
{ SHAPEWD3,    "shape.wd3" },
{ SHAPEWD4,    "shape.wd4" },
{ SHAPEWD5,    "shape.wd5" },
{ SHAPEWD6,    "shape.wd6" },
{ SHAPEWD7,    "shape.wd7" },
{ SMAPWDF,     "smap.wdf" },
{ SOUNDWDF,    "sound.wdf" },
{ STOCKWDF,    "stock.wdf" },
{ WADDONWDF,   "waddon.wdf" },
{ WZIFEWDF,    "wzife.wdf" },
{ WZIFEWD1,    "wzife.wd1" }
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
#define REG_ENUM(e)  (lua_pushinteger(L, e),lua_setglobal(L, #e))
	REG_ENUM(ADDONWDF);
	REG_ENUM(ATOMWDF);
	REG_ENUM(CHATWDF);
	REG_ENUM(FIREWORKWDF);
	REG_ENUM(GOODSWDF);
	REG_ENUM(ITEMWDF);
	REG_ENUM(MAGICWDF);
	REG_ENUM(MAPANIWDF);
	REG_ENUM(MHIMAGEWDF);
	REG_ENUM(MISCWDF);
	REG_ENUM(MUSICWDF);
	REG_ENUM(SCENEWDF);
	REG_ENUM(SHAPEWDF);
	REG_ENUM(SHAPEWD1);
	REG_ENUM(SHAPEWD2);
	REG_ENUM(SHAPEWD3);
	REG_ENUM(SHAPEWD4);
	REG_ENUM(SHAPEWD5);
	REG_ENUM(SHAPEWD6);
	REG_ENUM(SHAPEWD7);
	REG_ENUM(SMAPWDF);
	REG_ENUM(SOUNDWDF);
	REG_ENUM(STOCKWDF);
	REG_ENUM(WADDONWDF);
	REG_ENUM(WZIFEWDF);
	REG_ENUM(WZIFEWD1);
#undef REG_ENUM


	script_system_register_function(L, resource_manager_init);
	script_system_register_function(L, resource_manager_update);
	script_system_register_function(L, resource_manager_deinit);
	
}
