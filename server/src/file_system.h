#pragma once


#include <string>
#include <vector>
using String = std::string;
class FileSystem 
{
public:
	FileSystem();
	~FileSystem();

	static std::string GetPath();
	static String GetTSVPath(String name);
	static String GetTablePath(String name);
	static std::string GetAbsPath(std::string localPath);
	static std::string GetResourcePath(std::string localPath);
	static std::string GetAssetsPath(std::string path);
	static std::string GetShaderPath(std::string path);
	static std::string GetLuaPath(std::string path);
	static std::string GetWDFPath(std::string path);
	static std::string GetMapPath(std::string path);
	static std::string GetFontPath(std::string path);
	static std::string GetIconPath(std::string path);

	static std::vector<std::string> ListFiles(std::string path);
	static std::vector<std::string> ListAllFiles(std::string path);
};

