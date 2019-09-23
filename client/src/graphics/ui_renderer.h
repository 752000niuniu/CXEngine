#pragma once
#include "singleton.h"
#include <lua.hpp>
#include <NESupport.h>
#include "resource_manager.h"
#include "utils.h"
#include "nanovg.h"

class BaseSprite;

class UIObject
{
public:
	virtual void Draw() {};
};

class NEImageView : public UIObject
{
public:
	NEImageView(uint64_t resoureID = 0, std::vector<PalSchemePart> * patMatrix = nullptr) ;
	NEImageView(uint32_t pkg, uint32_t wasID, std::vector<PalSchemePart>* patMatrix = nullptr) : NEImageView(RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID), patMatrix) {};
	~NEImageView();

	virtual void Draw() override;
	BaseSprite* GetBaseSprite() { return m_pBS; }
private:
	map<int, int> m_ImageIDs;
	BaseSprite* m_pBS;
};

class UITextView : public UIObject
{
public:
	UITextView() ;
	~UITextView() {};
	virtual void Draw() override;
	CXString Text;
	CXString Font;
	float Size;
	float X;
	float Y;
	NVGcolor Color;
	NVGcolor BGColor;
	float Width;
	int Align;
};


class UIRenderer : public Singleton<UIRenderer>
{
public:
	UIRenderer();
	~UIRenderer();

	void Update();
	void Draw();
	
	void AddToDraw(UIObject* obj);
private:
	vector<UIObject*> m_Objects;
};

void luaopen_ui_renderer(lua_State* L);