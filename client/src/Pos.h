#pragma once
#include "defines.h"
struct Pos
{
    float x,y;
    Pos()
    {}
    Pos(float x, float y)
    {
        this->x =x;
        this->y = y;
    }
};

struct IntPos
{
	int32 x, y;
};

struct BoxPos
{
	int32 x, y;
};

struct Bound
{
	float left;
	float right;
	float top;
	float bottom;
};