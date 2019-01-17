#pragma once
#include "defines.h"
#include "core/Texture.h"
#include "mesh.h"
class Model
{
public:
	Model(std::string path, bool gamma = false);
	~Model();
	void LoadModel();
	void Draw(Shader* shader);
private:
	void ProcessNode(aiNode* node, const aiScene * scene);
	Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture*> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	std::vector<Texture*> m_Textures;
	std::vector<Mesh*> m_Meshes;
	std::string m_Dir;
	bool m_GammaCorrection;
	std::string m_Path;

};
