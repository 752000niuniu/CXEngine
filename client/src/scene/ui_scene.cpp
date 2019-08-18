#include "ui_scene.h"
#include "scene/scene_manager.h"
#include "scene/was_viewer_scene.h"
#include "graphics/graphics_2d.h"
#include "graphics/Renderer.h"
#include "resource_manager.h"
#include "cxrandom.h"
#include "profile.h"
#include "Time/Time.h"
#include "graphics/bitmap.h"
#include "input_manager.h"
#include "cxmath.h"
#include "graphics/object_2d.h"
#include "animation/frame_animation.h"
#include "text_renderer.h"


float tv_start = -1.f;
float tv_end = -1.f;
struct UIButton : public View
{
	Rect* rect;
	UIButton(FrameAnimation* f)
	{
		m_Frame = f;

		rect = new Rect(f->GetPos().x, f->GetPos().y, f->GetWidth(), f->GetHeight());
		rect->Color() = Vec4i(RANDOM_INSTANCE->NextInt(0, 255), RANDOM_INSTANCE->NextInt(0, 255), 0, 255);
		RENDERER_2D_INSTANCE->AddObject(rect);
		INPUT_MANAGER_INSTANCE->RegisterView(this);
	}
	
	bool CheckDrag(int dx, int dy)
	{
		return pow(dx, 2) + pow(dy, 2) >= 16;
	}

	void OnDragStart()
	{
		auto pos = m_Frame->GetPos();
		std::cout << "OnDragStart x=" << pos.x << " y=" << pos.y << std::endl;
	}

	void OnDragMove(int dx, int dy)
	{
		auto pos = m_Frame->GetPos();
		pos.x += dx;
		pos.y += dy;
		m_Frame->SetPos(pos);
		
		std::cout << "OnDragMove x=" << pos.x << " y=" << pos.y << std::endl;
	}

	void OnDragEnd()
	{
		auto pos = m_Frame->GetPos();
		std::cout << "OnDragEnd x=" << pos.x << " y=" << pos.y << std::endl;
	}

	~UIButton()
	{
		INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	}

	void OnClick(int button, int x, int y)
	{
		std::cout << "clicked" << std::endl;
	}; 

	Bound GetViewBounds() const
	{
		return m_Frame->GetBound();
	};

	int GetViewLayer()const 
	{
		return 1;
	};

	FrameAnimation* m_Frame;
	int lastx, lasty;
};


UIScene::UIScene(int id, String name)
	:BaseScene(id, name)
{

}

UIScene::~UIScene()
{

}

std::vector< Button*> pButtons;
FrameAnimation* test;
Rect* rect1;
Rect* rect2;
Texture* cursor;
TextView * s_TextView = nullptr;
TextView * s_TextView2 = nullptr;


float t = 0.f;
Line* s_Line;


std::vector<Line*> s_BezierCurve;
void UIScene::OnLoad()
{
	/*s_Line = new Line({ 0,0 }, {100,100});
	RENDERER_2D_INSTANCE->AddObject(s_Line);*/
	
	s_TextView = new TextView();
	s_TextView->X = 20;
	s_TextView->Y = 120;
	s_TextView->PaddingHorizontal = 10;
	s_TextView->PaddingVertical = 22;
	s_TextView->Background = nullptr;

	s_TextView2 = new TextView();
	s_TextView2->X = 400;
	s_TextView2->Y = 400;
	s_TextView2->Background = nullptr;
	s_TextView2->PaddingHorizontal = 10;
	s_TextView2->PaddingVertical = 22;
	
}

void UIScene::OnUnLoad()
{

}

float lastx = -1.f;
float lasty = -1.f;

void UIScene::Update()
{
	
	if (s_TextView)
		s_TextView->OnUpdate();
	if (s_TextView2)
		s_TextView2->OnUpdate();
	
	


	
}

void UIScene::Draw()
{
	if (s_TextView)
		s_TextView->OnDraw();
	if (s_TextView2)
	//	s_TextView2->OnDraw();

	RENDERER_2D_INSTANCE->Render();
}
