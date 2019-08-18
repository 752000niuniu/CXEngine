#pragma once
#include "pos.h"

class Texture;
class InputManager;

class View
{
public:
	friend InputManager;

	virtual void OnClick(int button, int x, int y) {};
	virtual Bound GetViewBounds()  = 0;
	virtual int GetViewLayer()const { return 1; };
	virtual bool CheckDrag(int x, int y) { return false; };
	virtual void OnDragStart() { };
	virtual void OnDragMove(int x, int y) {};
	virtual void OnDragEnd() {};
	virtual void OnHover(float x,float y) {};
	virtual void OnGlobleClick(int x, int y) {};
	virtual void OnCharacterInput(uint32_t charcode) {};
	virtual void OnFocusChanged(bool focus) {};

	virtual void OnKeyDownEvent(int keyCode) {};
	virtual void OnKeyUpEvent(int keyCode) {};
	virtual void OnKeyRepeatEvent(int keyCode) {};

	View() :dragStart(false), pressed(false), dragX(-1), dragY(-1), button(-1) { }
	virtual ~View() {};
private:
	void SetDragStart(bool isstart) { dragStart = isstart; }
	bool IsDragStart() { return dragStart; }
	void SetPressed(bool ispress, int button) { pressed = ispress; this->button = button; }
	bool IsPressed() { return pressed; }
	int GetPressedButton() { return button; }

	bool dragStart;
	bool pressed;
	int button;
	int dragX;
	int dragY;
};




class ImageView : public View
{
public:
	int X;
	int Y;
	int Width;
	int Height;
	uint64_t BackgroundResID;
	Texture* Background;
	ImageView(uint32_t pkg, uint32_t wasID);
	virtual ~ImageView();

	void OnUpdate();

	void OnDraw();
	
	void OnClick(int button, int x, int y) override;

	Bound GetViewBounds()  override;
	int GetViewLayer()const override;

	bool CheckDrag(int dx, int dy) override;

	void OnDragMove(int dx, int dy) override;

	void OnHover(float x, float y) override;
private:
	float m_HoverX;
	float m_HoverY;
};


void luaopen_ui(lua_State* L);