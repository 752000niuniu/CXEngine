#include "ui.h"
#include "file_system.h"
#include "input_manager.h"
#include "graphics/bitmap.h"
#include "graphics/texture.h"
#include "resource_manager.h"
#include "text_renderer.h"
#include "Time/Time.h"
#include "sprite_renderer.h"
#include "window.h"
#include "scene/scene_manager.h"
#include "scene/scene.h"
#include "utils.h"

void TextView::OnCharacterInput(uint32_t charcode)
{
	TextCache.push_back(charcode);
}

void TextView::OnClick(int button, int x, int y)
{
	INPUT_MANAGER_INSTANCE->RequestFocus(this);
};

Bound TextView::GetViewBounds() 
{
	return { (float)X,(float)X + (float)Width,(float)Y,(float)Y + (float)Height };
};

int TextView::GetViewLayer()const
{
	return 1;
};

bool TextView::CheckDrag(int dx, int dy)
{
	return pow(dx, 2) + pow(dy, 2) >= 16;
}

void TextView::OnDragMove(int dx, int dy)
{
	X += dx;
	Y += dy;
}

void TextView::OnFocusChanged(bool focus)
{
	m_IsEditing = focus;
}

TextView::TextView()
	:m_LastTime(0),
	m_Alpha(1.f),
	m_Bounce(false),
	X(0),
	Y(0),
	Width(0),
	Height(0),
	PaddingHorizontal(0),
	PaddingVertical(0),
	Color(1.f),
	Background(nullptr),
	m_IsEditing(false),
	OnEnterHit(nullptr),
	ShowEmotion(false)
{
	BitmapFile bpfile;
	Bitmap::Load(FileSystem::GetIconPath("text_cursor_w.bmp"), bpfile);
	Cursor = new Texture(bpfile.infoHeader.biWidth, bpfile.infoHeader.biHeight, true, bpfile.imageData);
	if (bpfile.imageData != nullptr)
	{
		delete bpfile.imageData;
		bpfile.imageData = nullptr;
	}
	BackgroundResID = RESOURCE_MANAGER_INSTANCE->EncodeWAS(WzifeWDF, 0x39D3BD99);
	INPUT_MANAGER_INSTANCE->RegisterView(this);

}

TextView::~TextView()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	SafeDelete(Cursor);
	SafeDelete(Background);
}


void TextView::SetBackground(uint64_t resID)
{
	BackgroundResID = resID;
}

void TextView::RefreshText()
{
	Text = std::wstring(TextCache.begin(), TextCache.end());
	m_MeasureInfo = TextRenderer::GetInstance()->MeasureText(Text, Width - PaddingHorizontal * 2);
}

void TextView::OnKeyDownEvent(int keyCode)
{
	if (m_IsEditing)
	{
		if (keyCode == GLFW_KEY_ENTER)
		{
			if(OnEnterHit)
			{
				OnEnterHit();
			}
			else
			{
				TextCache.push_back('\n');
				RefreshText();
			}
		}
		else if (keyCode == GLFW_KEY_BACKSPACE)
		{
			if (TextCache.size() > 0)
				TextCache.pop_back();
		}
		else if(keyCode == GLFW_KEY_DELETE)
		{
			TextCache.clear();
		}
	}
}

void TextView::OnKeyRepeatEvent(int keyCode)
{
	if (m_IsEditing)
	{
		if (keyCode == GLFW_KEY_BACKSPACE)
		{
			if (TextCache.size() > 0)
				TextCache.pop_back();
		}
	}
}

void TextView::DrawCenter(float x, float y)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void TextView::OnUpdate()
{
	// drawBackgroud
	auto backgroud = RESOURCE_MANAGER_INSTANCE->LoadWASSpriteByID(BackgroundResID);
	if (backgroud != nullptr && Background == nullptr)
	{
		auto& frame = backgroud->mFrames[0];
		Background = new Texture(frame.width, frame.height, true, (uint8*)frame.src.data());
		Width = frame.width;
		Height = frame.height;
	}
	auto now = std::chrono::system_clock::now();
	uint64_t currentTime = now.time_since_epoch().count() / 10000;

	if (m_LastTime == 0)
	{
		m_LastTime = currentTime;
	}

	if (currentTime - m_LastTime > 500)
	{
		m_LastTime = currentTime;
		m_Bounce = !m_Bounce;
		RefreshText();
	}
	else
	{
		if (m_Bounce)
		{
			if (currentTime - m_LastTime < 300)
				m_Alpha = ((currentTime - m_LastTime)) / 300.f;
			else
				m_Alpha = 1.f;
		}
		else
		{
			if (currentTime - m_LastTime < 300)
				m_Alpha = 1.f - ((currentTime - m_LastTime)) / 300.f;
			else
				m_Alpha = 0.f;
		}
	}
}

void TextView::OnDraw()
{
	// drawBackgound
	if (Background != nullptr)
		SPRITE_RENDERER_INSTANCE->DrawFrameSprite(
			Background,
			{ X ,Y },
			{ Width,Height });

	// drawText
	TextRenderer::GetInstance()->DrawTextW(
		Text,
		X + PaddingHorizontal,
		Y + PaddingVertical,
		TextRenderer::LEFT,
		Width - PaddingHorizontal * 2,
		Color,
		ShowEmotion);

	// drawCursor
	if (Cursor && m_IsEditing)
		SPRITE_RENDERER_INSTANCE->DrawBitmap(
			Cursor,
			{ X + PaddingHorizontal + m_MeasureInfo.LastLineWidth,
			Y + PaddingVertical + m_MeasureInfo.Height - TextRenderer::GetInstance()->GetFontHeight() },
			{ Cursor->GetWidth() ,Cursor->GetHeight() },
			{ 1.f,1.f,1.f },
			m_Alpha
		);
}


Button::Button()
{

}

Button::~Button()
{

}

void Button::OnUpdate()
{

}

void Button::OnDraw()
{

}

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
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(FileSystem::GetFontPath("msyh.ttf").c_str(), 14.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
}

void luaopen_ui(lua_State* L)
{
	script_system_register_function(L, imgui_init);
}