#pragma once

#include "defines.h"
#include "core/Texture.h"

class Mesh
{
public:

	struct Vertex {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
	};

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture*> textures);
	~Mesh();
	void GenGPUData();
	void Draw(Shader* shader);
private:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	std::vector<Texture*> m_Textures;
	uint32_t m_VAO;
	uint32_t m_VBO;
	uint32_t m_EBO;

};
