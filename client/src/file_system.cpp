#include "file_system.h"
#if defined(_WIN32)
#include <corecrt_io.h>
#endif

FileSystem::FileSystem()
{

};

FileSystem::~FileSystem()
{
	
};

std::string FileSystem::GetPath()
{
	return WORK_DIR;
}

String FileSystem::GetTSVPath(String name)
{
	return GetPath() + "/res/tables/" + name + ".tsv";
}

String FileSystem::GetTablePath(String name)
{
	return GetPath() + "/res/tables/" + name;
}

std::string FileSystem::GetAbsPath(std::string localPath)
{
	return GetPath() + "/" + localPath;
}

std::string FileSystem::GetResourcePath(std::string localPath)
{
	return GetPath() + "/res/" + localPath;
}

std::string FileSystem::GetAssetsPath(std::string path)
{
	return GetPath() + "/res/assets/" + path;
}

std::string FileSystem::GetShaderPath(std::string path)
{
	return GetPath() + "/res/shader/" + path;
}
std::string FileSystem::GetLuaPath(std::string path)
{
	//return GetPath() + "/external/lib/lua/" + path;
	return GetPath() + "/scripts/client/" + path;
}
std::string FileSystem::GetWDFPath(std::string path)
{
	return GetPath() + "/res/data/" + path;
}
std::string FileSystem::GetMapPath(std::string path)
{
	return GetPath() + "/res/data/scene/" + path + ".map";
}
std::string FileSystem::GetFontPath(std::string path)
{
	return GetPath() + "/res/font/" + path;
}
std::string FileSystem::GetIconPath(std::string path)
{
	return GetPath() + "/res/icon/" + path;
}

std::vector<std::string> FileSystem::ListFiles(std::string path)
{
#if defined(_WIN32)
    std::vector<std::string> files;
    path.append("\\*.*");
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(path.c_str(), &findData);
    if (handle == -1)
        return files;

    int limit = 1000;
    int found = 0;
    do
    {
        files.push_back(findData.name);
        found++;
    } while (_findnext(handle, &findData) == 0 || found >= limit);

    _findclose(handle);
    return files;
#else
    return {};
#endif
}

std::vector<std::string> FileSystem::ListAllFiles(std::string path)
{
#if defined(_WIN32)
    std::vector<std::string> files;
    path.append("\\*.*");
    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(path.c_str(), &findData);
    if (handle == -1)
        return files;

    int limit = 1000;
    int found = 0;
    do
    {
        if (findData.attrib & _A_SUBDIR)
        {
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
                continue;
            //cout << findData.name << "\t<dir>\n";
            auto nextFiles = ListAllFiles(path + "\\" + findData.name);
            for (auto f : nextFiles)
            {
                files.push_back(f);
                found++;
            }
        }
        else
        {
            files.push_back(findData.name);
            found++;
        }
    } while (_findnext(handle, &findData) == 0 || found >= limit);
    
    _findclose(handle);
    return files;
#else
    return {};
#endif
}


std::string fs_get_tsv_path(const char* name)
{
	return FileSystem::GetTSVPath(name);
}

void luaopen_filesystem(lua_State*L)
{
	script_system_register_function(L, fs_get_tsv_path);

}