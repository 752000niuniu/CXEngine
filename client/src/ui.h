#pragma once
#include "script_system.h"
#include "text_renderer.h"
#include "Pos.h"

class InputManager;

class View
{
public:
	friend InputManager;

	virtual void OnClick(int button, int x, int y) = 0;
	virtual Bound GetViewBounds()  = 0;
	virtual int GetViewLayer() const = 0;
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


class TextView : public View
{
public:
	int X;
	int Y;
	int Width;
	int Height;
	uint64_t BackgroundResID;
	Texture* Background;
	Texture* Cursor;
	glm::vec3 Color;
	int PaddingHorizontal;
	int PaddingVertical;
	bool ShowEmotion;
	std::wstring Text;
	std::vector<uint32_t> TextCache;

	void OnCharacterInput(uint32_t charcode) override;
	void OnClick(int button, int x, int y) override;

	Bound GetViewBounds()  override;
	int GetViewLayer()const override;

	bool CheckDrag(int dx, int dy) override;

	void OnDragMove(int dx, int dy) override;

	void OnFocusChanged(bool focus) override;

	void OnKeyDownEvent(int keyCode) override;
	void OnKeyRepeatEvent(int keyCode) override;

	std::function<void()> OnEnterHit;
	void DrawCenter(float param1, float param2);
private:
	

	uint64_t m_LastTime;
	float m_Alpha;
	bool m_Bounce;
	bool m_IsEditing;
	TextRenderer::MeasureInfo m_MeasureInfo;

public:
	TextView();
	virtual ~TextView();

	void SetBackground(uint64_t resID);
	void RefreshText();
	
	
	void OnUpdate();

	void OnDraw();
};

class Button : public TextView
{
public:
	Button();
	virtual ~Button();

	void OnUpdate();

	void OnDraw();
	
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