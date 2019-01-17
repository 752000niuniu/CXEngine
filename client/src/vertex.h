#pragma once
#include "defines.h"

class VertexFactory : public Singleton<VertexFactory>
{
public:
	VertexFactory();
	~VertexFactory();
	std::vector<float> CreateCubic();
private:
	
};

#define VERTEX_FACTORY_INSTANCE VertexFactory::GetInstance()