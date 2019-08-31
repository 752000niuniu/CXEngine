#include "ui.h"
#include "input_manager.h"
#include "resource_manager.h"
#include "utils.h"
#include "window.h"
#include "sprite_renderer.h"
#include "scene/scene.h"
#include "scene/scene_manager.h"
#include "file_system.h"


ImageView::ImageView(uint32_t pkg, uint32_t wasID)
{
	INPUT_MANAGER_INSTANCE->RegisterView(this);
	Background = nullptr;
	BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(pkg, wasID);
	X = Y = Width = Height = 0;
	m_HoverX = m_HoverY = 0;
}

ImageView::~ImageView()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(Background);
}

void ImageView::OnUpdate()
{
	auto backgroud = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(BackgroundResID);
	if (backgroud != nullptr && Background == nullptr)
	{
		auto& frame = backgroud->mFrames[0];
		Background = new Texture(frame.width, frame.height, true, (uint8*)frame.src.data());
		Width = frame.width;
		Height = frame.height;
		X = (int) WINDOW_INSTANCE->GetCenterX() - Width / 2;
		Y = (int)WINDOW_INSTANCE->GetCenterY() - Height/ 2;;
	}
}

void ImageView::OnDraw()
{
	// drawBackgound
	if (Background != nullptr)
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(
			Background,
			{ X ,Y },
			{ Width,Height });
}

void ImageView::OnClick(int button, int x, int y)
{
	dynamic_cast<Scene*>(SCENE_MANAGER_INSTANCE->GetCurrentScene())->
		OnSmapClick(m_HoverX, m_HoverY, (float)Width - 18.f * 2,( float)Height - 15.f * 2);
}

Bound ImageView::GetViewBounds()
{
	return { (float)X,(float)X + (float)Width,(float)Y,(float)Y + (float)Height };
}

int ImageView::GetViewLayer() const
{
	return 1;
}

bool ImageView::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void ImageView::OnDragMove(int dx, int dy)
{
	X += dx;
	Y += dy;
}


void ImageView::OnHover(float x, float y)
{
	std::cout << "OnHover " << x - X - 18 << " : " << y - Y - 15 << std::endl;
	m_HoverX = x - X - 18;
	m_HoverY = y - Y - 15;
}

void imgui_init()
{
	
}

void luaopen_ui(lua_State* L)
{
	script_system_register_function(L, imgui_init);
}