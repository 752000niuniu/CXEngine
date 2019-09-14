#pragma once

#include "define_types.h"
#include "actor_enum.h"

class ActorProp
{
public:
	EActorPropType  type;
	union 
	{
		int i;
		bool b;
		float f;
		float v2[2];
		uint64_t l;
		CXString s;
	};

	ActorProp(int v = 0) : type(PROP_TYPE_INT), i(v) {};
	ActorProp(uint64_t v) : type(PROP_TYPE_UINT64), l(v) {};
	ActorProp(bool v) : type(PROP_TYPE_BOOL), b(v) {};
	ActorProp(float v) : type(PROP_TYPE_FLOAT), f(v) {};
	ActorProp(float  v[2]) : type(PROP_TYPE_VEC2) { v2[0] = v[0]; v2[1] = v[1]; };
	ActorProp(const CXString& v) : type(PROP_TYPE_STR) { new(&s) CXString(v); };
	ActorProp(const ActorProp& prop) : type(prop.type) { copyUnion(prop); };

	ActorProp& operator=(const ActorProp& prop);
	~ActorProp() {
		if (type == PROP_TYPE_STR)s.~CXString();
	};

	ActorProp& operator=(bool);
	ActorProp& operator=(int);
	ActorProp& operator=(float);
	ActorProp& operator=(float v2[2]);
	ActorProp& operator=(uint64_t);
	ActorProp& operator=(const CXString&);
private:
	void copyUnion(const ActorProp& prop);
};


int actor_set_prop(lua_State* L);
int actor_get_prop(lua_State* L);
int actor_reg_prop(lua_State* L);