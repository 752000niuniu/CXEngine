#pragma once
#include "../defines.h"
#include "../defineTypes.h"

class Transform
{
public:
    Transform();
    
    ~Transform();
//     Vec3& pos(){return pos;}
//     Vec3& size(){return size;}
//     Vec3& scale(){return scale;}
//     Vec3& rotation(){return rotation;}
//     Vec3& center(){return rotation;}
//     int& layer(){return layer;}
    
// private:
    glm::tvec3<int> pos;
	glm::tvec3<int> size;
	glm::tvec3<int> scale;
	glm::tvec3<int> rotation;
	glm::tvec3<int> center;
    int layer;
};