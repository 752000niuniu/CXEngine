#include "resource_manager.h"
#include "graphics/image.h"
#include "file_system.h"
#include "profile.h"
#include "file_loading.h"
#include <string>
#include "logger.h"
#include "tsv.h"
#include "actor/action.h"
#include "texture_manager.h"
#include "game.h"




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


static std::vector<String> s_PackToName = { "addon.wdf","atom.wdf","chat.wdf","firework.wdf","goods.wdf","item.wdf","magic.wdf","mapani.wdf","mhimage.wdf","misc.wdf","music.wdf","scene.wdf","shape.wd1","shape.wd2","shape.wd3","shape.wd4","shape.wd5","shape.wd6","shape.wd7","shape.wdf","smap.wdf","sound.wdf","stock.wdf","waddon.wdf","wzife.wd1","wzife.wdf","wzimage.wdf" };
static std::map<uint32_t, NE::WDF*> s_Loaders;
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

bool utils_is_pal_scheme_part_equal(std::vector<PalSchemePart> pat1, std::vector<PalSchemePart> pat2) 
{
	size_t sz1 = pat1.size() * sizeof(PalSchemePart);
	size_t sz2 = pat2.size() * sizeof(PalSchemePart);
	if (sz1 != sz2)return false;
	return memcmp(pat1.data(), pat2.data(), sz1) ==0;
}


ResourceManager::ResourceManager()
	:Singleton<ResourceManager>(),
	m_AvatarRoleTSV(FileSystem::GetTablePath("avatar_role.tsv")),
	m_AvatarWeaponTSV(FileSystem::GetTablePath("avatar_weapon.tsv")),
	m_AvatarNpcTSV(FileSystem::GetTablePath("avatar_npc.tsv"))
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::Clear()
{
    
}

Sprite* ResourceManager::LoadWASSpriteByID(uint64_t resID, bool sync, std::vector<PalSchemePart>* patMatrix)
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
			return s_Loaders[pack]->LoadSprite(wasID, patMatrix);
		}
		else
		{
			iothread->PostTask(path.c_str(), [this, pack, wasID, patMatrix](const char* path)->bool
			{
				if (s_Loaders.find(pack) == s_Loaders.end())
				{
					s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
				}
				s_Loaders[pack]->LoadSprite(wasID, patMatrix);
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


Sprite* ResourceManager::LoadWASSprite(uint32_t pack, uint32 wasID,bool sync, std::vector<PalSchemePart>* patMatrix )
{
	auto resID = EncodeWAS(pack, wasID);
	return LoadWASSpriteByID(resID,sync, patMatrix);
}

void ResourceManager::UnLoadWASSprite(uint32_t pack, uint32 wasID)
{
	auto resID = EncodeWAS(pack, wasID);
	return UnLoadWASSpriteByID(resID);
}


void ResourceManager::OnUpdate()
{

}

uint64_t ResourceManager::GetActionResID(int type, int roleID, int actionID)
{
	if (actionID < 0) return 0;
	if (roleID < 0) return 0;

	utils::tsv* rowTable = FindAvatarTable(type);
	std::string wasIDstr("");
	if (actionID == ACTION_BATIDLE)
	{
		auto wasIdle = rowTable->Rows[roleID][action_get_name(ACTION_IDLE)];
		auto wasBatidle = rowTable->Rows[roleID][action_get_name(ACTION_BATIDLE)];
		if (wasBatidle != "")wasIDstr = wasBatidle;
		else wasIDstr = wasIdle;
	}
	else if (actionID == ACTION_IDLE) {

		auto wasIdle = rowTable->Rows[roleID][action_get_name(ACTION_IDLE)];
		auto wasBatidle = rowTable->Rows[roleID][action_get_name(ACTION_BATIDLE)];
		if (wasIdle != "")wasIDstr = wasIdle;
		else wasIDstr = wasBatidle;
	}
	else
	{
		wasIDstr = rowTable->Rows[roleID][action_get_name(actionID)];
	}
	if (wasIDstr == "")
	{
		return 0;
	}

	auto ids = utils::split_by_cnt(wasIDstr, ',', 1);
	if (ids.size() == 0)return 0;
	auto resid = utils::split_by_cnt(ids[0], '-', 2);

	uint32 pack_index = std::stoul(resid[0], 0);
	uint32 wasID = std::stoul(resid[1], 0, 16);
	return RESOURCE_MANAGER_INSTANCE->EncodeWAS(pack_index, wasID);
}

uint64_t ResourceManager::GetActionResID(int type, CXString id, int action)
{
	if (type >= AVATAR_TYPE_COUNT || type < 0)return 0;
	utils::tsv* tbl = nullptr;
	if (type == AVATAR_TYPE_ROLE)
		tbl = &m_AvatarRoleTSV;
	else if (type == AVATAR_TYPE_WEAPON)
		tbl = &m_AvatarWeaponTSV;
	else if (type == AVATAR_TYPE_NPC)
		tbl = &m_AvatarNpcTSV;
	assert(tbl != nullptr);
	
	std::string idstr = tbl->MapRows[id][action_get_name(action)];
	if(idstr==""){
		if(action==ACTION_IDLE){
			idstr = tbl->MapRows[id][action_get_name(ACTION_BATIDLE)];
		}else if(action == ACTION_BATIDLE){
			idstr = tbl->MapRows[id][action_get_name(ACTION_IDLE)];
		}
	}
	if (idstr == "")return 0;
	
	auto ids = utils::split_by_cnt(idstr, ',', 1);
	if (ids.size() == 0)return 0;
	auto first_res_id= utils::split_by_cnt(ids[0], '-', 2);

	uint32 pack = std::stoul(first_res_id[0], 0);
	uint32 wasID = std::stoul(first_res_id[1], 0, 16);
	return EncodeWAS(pack, wasID);
}

uint64_t ResourceManager::GetWeaponResID(int weaponID, int actionID)
{
	if (actionID < 0) return 0;
	if (weaponID < 0) return 0;

	std::string wasIDstr("");
	if (actionID == ACTION_BATIDLE)
	{
		auto wasIdle = m_AvatarWeaponTSV.Rows[weaponID][action_get_name(ACTION_IDLE)];
		auto wasBatidle = m_AvatarWeaponTSV.Rows[weaponID][action_get_name(ACTION_BATIDLE)];
		if (wasIdle != "")wasIDstr = wasBatidle;
		else wasBatidle = wasIdle;
	}
	else
	{
		wasIDstr = m_AvatarWeaponTSV.Rows[weaponID][action_get_name(actionID)];
	}
	if (wasIDstr == "")return 0;

	auto ids = utils::split_by_cnt(wasIDstr, ',', 1);
	if (ids.size() == 0)return 0;
	auto resid = utils::split_by_cnt(ids[0], '-', 2);

	uint32 pack_index = std::stoul(resid[0], 0);
	uint32 wasID = std::stoul(resid[1], 0, 16);
	return RESOURCE_MANAGER_INSTANCE->EncodeWAS(pack_index, wasID);
}


int ResourceManager::GetRoleIDByName(int actorType, const char* templ_name)
{
	utils::tsv* rowTable = FindAvatarTable(actorType);
	for (size_t i = 0; i < rowTable->Rows.size(); i++)
	{
		auto& row = rowTable->Rows[i];
		if (row["ID"] == templ_name)
		{
			return (int)i;
		}
	}
	return 0;
}

utils::tsv* ResourceManager::FindAvatarTable(int actor_type)
{
	utils::tsv* rowTable = nullptr;
	switch (actor_type)
	{
	case ACTOR_TYPE_DEFAULT:
		rowTable = &m_AvatarNpcTSV;
		break;
	case ACTOR_TYPE_PLAYER:
		rowTable = &m_AvatarRoleTSV;
		break;
	case ACTOR_TYPE_PET:
	case ACTOR_TYPE_NPC:
		rowTable = &m_AvatarNpcTSV;
		break;
	default:
		break;
	}
	return rowTable;
}
int ResourceManager::ActorTypeToAvatarType(int actorType)
{
	if (actorType == ACTOR_TYPE_DEFAULT || actorType == ACTOR_TYPE_PLAYER)
		return AVATAR_TYPE_ROLE;
	if (actorType == ACTOR_TYPE_PET || actorType == ACTOR_TYPE_NPC)
		return AVATAR_TYPE_NPC;
	return AVATAR_TYPE_ROLE;
}

int ResourceManager::GetRoleID(CXString id)
{
	return 0;
}

uint64_t ResourceManager::GetActorActionResID(int actorType, CXString roleID, int actionID)
{
	int type = ActorTypeToAvatarType(actorType);
	return GetActionResID(type, roleID, actionID);
}

uint64_t ResourceManager::GetWeaponActionResID(CXString id, int actionID)
{
	return GetActionResID(AVATAR_TYPE_WEAPON, id, actionID);
}

void ResourceManager::ExportWas(uint64_t id, CXString path)
{
	uint32_t pack, wasid;
	DecodeWAS(id, pack, wasid);
	s_Loaders[pack]->SaveWAS(wasid, path.c_str());
}


PalSpriteInfo* ResourceManager::LoadSprite(uint64_t resID, std::vector<PalSchemePart>* pat)
{
	auto it = m_Sprites.find(resID);
	if (it != m_Sprites.end()) {
		if (pat == nullptr) {
			//assert(it->second.size() == 1);
			return it->second[0];
		}
		else {
			auto& spinfos = it->second;
			for (auto* info : spinfos)
			{
				if (utils_is_pal_scheme_part_equal(*pat, info->pat)) {
					return info;
				}
			}
		}
	}

	uint32_t pack, wasid;
	DecodeWAS(resID, pack, wasid);
	PalSpriteInfo* info = new PalSpriteInfo();
	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
	}
	if(pat){
		info->sprite = s_Loaders[pack]->UnpackSprite(wasid, *pat);
		info->pat = *pat;
		info->pati =(int) m_Sprites[resID].size();
	}else{
		info->sprite = s_Loaders[pack]->UnpackSprite(wasid, {});
		info->pat = {};
		info->pati = 0;
	}
	
	m_Sprites[resID].push_back(info);
	return info;
}

void ResourceManager::UnLoadSprite(uint64_t resID)
{

}

int resource_get_action_id(lua_State* L)
{
	auto type = (int)lua_tointeger(L, 1);
	auto id = lua_tostring(L, 2);
	auto actionID = (int)lua_tointeger(L, 3);
	auto resid = RESOURCE_MANAGER_INSTANCE->GetActionResID(type, id, actionID);
	lua_pushinteger(L, resid);
	return 1;
}

int resource_get_weapon_id(lua_State* L)
{
	auto weaponID = (int)lua_tointeger(L, 1);
	auto actionID = (int)lua_tointeger(L, 2);
	auto wasid = RESOURCE_MANAGER_INSTANCE->GetWeaponResID(weaponID, actionID);
	lua_pushinteger(L, wasid);
	return 1;
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

int res_get_was(lua_State* L) {
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	const char* path = lua_tostring(L, 3);

	if (s_Loaders.find(pack) == s_Loaders.end())
	{
		s_Loaders[pack] = new NE::WDF(utils::GetPathByPackID(pack));
	}
	uint8_t* pData;
	size_t size;
	s_Loaders[pack]->LoadFileData(wasid, pData, size);
	NE::EFileType type = NE::check_file_type((char*)pData, size);
	lua_pushinteger(L, type);
	return 1;
}


int res_encode(lua_State* L) {
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);
	lua_pushinteger(L, RESOURCE_MANAGER_INSTANCE->EncodeWAS(pack, wasid));
	return 1;
}



int res_decode(lua_State* L) {
	uint64_t res = (uint64_t)lua_tointeger(L, 1);
	uint32_t pack = 0;
	uint32_t wasID = 0;
	RESOURCE_MANAGER_INSTANCE->DecodeWAS(res, pack, wasID);
	lua_pushinteger(L, pack);
	lua_pushinteger(L, wasID);
	return 2;
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

	REG_ENUM(AVATAR_TYPE_ROLE);
	REG_ENUM(AVATAR_TYPE_WEAPON);
	REG_ENUM(AVATAR_TYPE_NPC);
	REG_ENUM(AVATAR_TYPE_COUNT);
#undef REG_ENUM

	script_system_register_luac_function(L, res_encode);
	script_system_register_luac_function(L, res_decode);

	script_system_register_function(L, resource_manager_init);
	script_system_register_function(L, resource_manager_update);
	script_system_register_function(L, resource_manager_deinit);
	script_system_register_luac_function(L, resource_get_action_id);
	script_system_register_luac_function(L, resource_get_weapon_id);
	
	script_system_register_luac_function(L, res_get_was);
}
