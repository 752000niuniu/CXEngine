#include "ui_renderer.h"
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "window.h"
#include "file_system.h"
#include "input_manager.h"
#include "script_system.h"
#include "animation/sprite.h"
#include "logger.h"


static unordered_map<uint64_t, int> s_ImageCache;

uint64_t encode_image_cache_id(uint64_t resid, int index) 
{
	uint64_t index64 = index;
	uint64_t id = resid | index64 << 48;
	return id;
}

void decode_image_cache_id(uint64_t id, uint64_t& resid, int& index)
{
	resid = id & 0x00ffffff;
	index = id >> 48;
}

void decode_image_cache_id(uint64_t id, uint32_t& pack, uint32_t& wasid, int& index)
{
	uint64_t resid;
	decode_image_cache_id(id, resid, index);
	res_decode_was(resid, pack, wasid);
}

class UINEImageView  : public UIObject
{
public:
	UINEImageView(BaseSprite* sp) :m_BaseSprite(sp) {};
	~UINEImageView() {};
	void Draw() override;
private:
	BaseSprite* m_BaseSprite;
};

void UINEImageView::Draw()
{
	m_BaseSprite->Draw();
}


NVGcontext* vg = NULL;
UIRenderer::UIRenderer()
{
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		cxlog_info("Could not init nanovg.\n");
		return;
	}

	int res = 0;
	res = nvgCreateFont(vg, "MSYH", FileSystem::GetFontPath("msyh.ttf").c_str());
	assert(res >= 0);
	res = nvgCreateFont(vg, "MSHT", FileSystem::GetFontPath("msht.ttf").c_str());
	assert(res >= 0);
	res = nvgCreateFont(vg, "SIMSUN", FileSystem::GetFontPath("simsun.ttc").c_str());
	assert(res >= 0);
	m_Dialog = new NPCDialog();
}

UIRenderer::~UIRenderer()
{
	SafeDelete(m_Dialog);
	nvgDeleteGL3(vg);
}

void UIRenderer::Update()
{

}

void UIRenderer::Draw()
{
	
	//script_system_call_function(script_system_get_luastate(), "on_ui_renderer_draw");
	for (UIObject*& obj:m_Objects){
		if (obj&&!obj->MarkRemove) {
			obj->Draw();
		}
		else {
			delete obj;
			obj = nullptr;
		}
	}
	if (m_Objects.size() > 0) {
		vector<UIObject*> tmp;
		for (UIObject*& obj : m_Objects) {
			if (obj&&!obj->MarkRemove) {
				tmp.push_back(obj);
			}
		}
		m_Objects.swap(tmp);
	}
	m_Dialog->Draw();
	
}

void UIRenderer::AddToDraw(UIObject* obj)
{
	m_Objects.push_back(obj);
}

void UIRenderer::RemoveToDraw(UIObject* obj)
{
	obj->MarkRemove = true;
}

void UIRenderer::Clear()
{
	for(auto*& obj : m_Objects){
		delete obj;
		obj = nullptr;
	}
	m_Objects.clear();
}

NVGcontext* UIRenderer::GetContext()
{
	return vg;
}
void UIRenderer::Begin()
{
	float width = (float)WINDOW_INSTANCE->GetWidth();
	float height = (float)WINDOW_INSTANCE->GetHeight();
	nvgBeginFrame(vg, width, height, width* 1.0f / height);
}
void UIRenderer::End()
{
	nvgEndFrame(vg);
}
NEImageView::NEImageView(uint64_t resoureID /*= 0*/, std::vector<PalSchemePart>* patMatrix /*= nullptr*/)
{
	m_pBS = new BaseSprite(resoureID, patMatrix);
}

NEImageView::~NEImageView()
{
	delete m_pBS;
}

void NEImageView::Draw()
{
	int index = m_pBS->Dir * m_pBS->GroupFrameCount + m_pBS->CurrentFrame;
	auto& frame = m_pBS->m_pSprite->Frames[index];

	uint64_t id = encode_image_cache_id(m_pBS->ResID, index);
	auto it = s_ImageCache.find(id);
	int imageid = 0;
	if (it == s_ImageCache.end()) {
		imageid = nvgCreateImageRGBA(vg, frame.Width, frame.Height, 0, (unsigned char*)frame.Src.data());
		s_ImageCache.insert({ id,imageid });
	}
	else {
		imageid = it->second;
	}
	nvgSave(vg);
	float left = m_pBS->Pos.x - frame.KeyX;
	float top = m_pBS->Pos.y - frame.KeyY;
	float w = (float)m_pBS->Width;
	float h = (float)m_pBS->Height;
	float rot = 0.0f / 180.0f * NVG_PI;
	float pad = 10;

	NVGpaint imgPaint = nvgImagePattern(vg, left, top, w, h, rot, imageid, 1);
	nvgBeginPath(vg);
	/*left += pad;
	top += pad;
	w -= pad * 2;
	h -= pad * 2;*/
	nvgRect(vg, left, top, w, h);
	nvgFillPaint(vg, imgPaint);
	nvgFill(vg);
	nvgRestore(vg);
}


UITextView::UITextView()
	:Text(""),
	Font("MSYH"),
	Size(14.f),
	X(0),
	Y(0),
	Color(nvgRGBA(255, 255, 255, 255)),
	BGColor(nvgRGBA(0, 0, 0, 0)),
	Width(0),
	Align(NVG_ALIGN_LEFT | NVG_ALIGN_TOP)
{

}

void UITextView::Draw()
{
	if (Text.size() == 0)return;
	nvgSave(vg);
	float lineh;
	
	nvgFontSize(vg, Size);
	nvgFontFace(vg, Font.c_str()); 
	nvgTextAlign(vg, Align);
	nvgTextMetrics(vg, NULL, NULL, &lineh);

	const char* start;
	const char* end;
	start = Text.data();
	end = Text.data() + Text.size();
	float y = Y;
	if (Align & NVG_ALIGN_BOTTOM) {
		float bounds[4];
		nvgTextBoxBounds(vg, 0, 0, Width, Text.c_str(), 0, bounds);
		y = y - bounds[3];
	}

	float x = X;
	
	if (Width != 0) {
		nvgBeginPath(vg);

		if (BGColor.a != 0) {
			float bgx = x;
			if (Align&NVG_ALIGN_CENTER) {
				bgx = bgx - Width / 2;
			}
			float bgy = y - 8;
			float bounds[4];
			nvgTextBoxBounds(vg, bgx, bgy, Width, Text.c_str(), 0, bounds);

			nvgFillColor(vg, BGColor);
			nvgRoundedRect(vg, bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1] + 8 * 2, 8.f);
			nvgFill(vg);
		}
		

		NVGtextRow rows[3];
		int nrows, lnum = 0;
		while ((nrows = nvgTextBreakLines(vg, start, end, Width, rows, 3))) {
			for (int i = 0; i < nrows; i++) {
				NVGtextRow* row = &rows[i];

				nvgFillColor(vg, Color);
				nvgText(vg, x, y, row->start, row->end);
				nvgFillColor(vg, Color);
				nvgText(vg, x, y, row->start, row->end);

				lnum++;
				y += lineh;
			}
			start = rows[nrows - 1].next;
		}
	}
	else {
		nvgFillColor(vg, Color);
		nvgText(vg, X, Y, Text.c_str(), NULL);

		nvgFillColor(vg, Color);
		nvgText(vg, X, Y, Text.c_str(), NULL);
	}
	nvgRestore(vg);
}

NPCDialog::NPCDialog()
{
	m_Visible = false;
	X = Y = 0;
	
	m_TvBG = new NEImageView(WZIFEWDF, 0x73D983B7);
	m_FaceImg = new NEImageView(WZIFEWDF, 0x7F84C945);
	m_Tv = new UITextView();
	
	m_Tv->Color = nvgRGB(255, 255, 255);
	m_Tv->Size = 17.f;
	m_Tv->Width = m_TvBG->GetBaseSprite()->Width - 32.f;

}

NPCDialog::~NPCDialog()
{
	SafeDelete(m_TvBG);
	SafeDelete(m_Tv);
	SafeDelete(m_FaceImg);
}

void NPCDialog::Draw()
{
	if (m_Visible) {
		float w = (float)WINDOW_INSTANCE->GetWidth();
		float h = (float)WINDOW_INSTANCE->GetHeight();

		auto* tvBGsp = m_TvBG->GetBaseSprite();
		tvBGsp->Pos.x = X + (w - tvBGsp->Width) / 2;
		tvBGsp->Pos.y = Y + 290;

		auto* facesp = m_FaceImg->GetBaseSprite();
		facesp->Pos.x = tvBGsp->Pos.x;
		facesp->Pos.y = tvBGsp->Pos.y - facesp->Height;

		m_Tv->X = tvBGsp->Pos.x + 16;
		m_Tv->Y = tvBGsp->Pos.y + 24;

		m_FaceImg->Draw();
		m_TvBG->Draw();
		m_Tv->Draw();
	}
}

void NPCDialog::SetText(const char* txt)
{
	m_Tv->Text = txt;
}

int ui_renderer_add_to_draw(lua_State*L){
	NEImageView* ptr = lua_check_pointer<NEImageView>(L, 1);
	UIRenderer::GetInstance()->AddToDraw(ptr);
	return 0;
}

int ne_imageview_get_base_sprite(lua_State*L){
	NEImageView* ptr = lua_check_pointer<NEImageView>(L, 1);
	BaseSprite* bs = ptr->GetBaseSprite();
	lua_push_base_sprite(L, bs);
	return 1;
}

luaL_Reg MT_NE_IMAGEVIEW[] =
{
	{ "GetBaseSprite",ne_imageview_get_base_sprite},
	{NULL,NULL}
};

int ne_imageview_create(lua_State* L)
{
	uint32_t pack = (uint32_t)lua_tointeger(L, 1);
	uint32_t wasid = (uint32_t)lua_tointeger(L, 2);

	NEImageView** ptr = (NEImageView * *)lua_newuserdata(L, sizeof(NEImageView));
	*ptr = new NEImageView(pack, wasid);
	if (luaL_newmetatable(L, "MT_NE_IMAGEVIEW")) {
		luaL_setfuncs(L, MT_NE_IMAGEVIEW, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2); 
	return 1;
}

int ui_textview_set_text(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	const char* txt = lua_tostring(L, 2);
	tv->Text = txt;
	return 0;
}

int ui_textview_set_pos(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float x = (float) lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);
	tv->X = x;
	tv->Y = y;
	return 0;
}


int ui_textview_set_color(lua_State* L)
{
#define break_color_rgba(c)  nvgRGBA((c>>24)& 0xff,(c& 0xff0000) >> 16,  (c& 0xff00) >> 8, (c& 0xff))
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	uint32_t color = (uint32_t)lua_tointeger(L, 2);
	tv->Color = break_color_rgba(color);
#undef break_color_rgba
	return 0;
}

int ui_textview_set_bg_color(lua_State* L)
{
#define break_color_rgba(c)  nvgRGBA((c>>24)& 0xff,(c& 0xff0000) >> 16,  (c& 0xff00) >> 8, (c& 0xff))
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	uint32_t color = (uint32_t)lua_tointeger(L, 2);
	tv->BGColor = break_color_rgba(color);
#undef break_color_rgba
	return 0;
}

int ui_textview_set_text_size(lua_State* L)
{
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float size = (float)lua_tonumber(L, 2);
	tv->Size = size;
	return 0;
}

int ui_textview_set_width(lua_State*L){
	UITextView* tv = lua_check_pointer<UITextView>(L, 1);
	float width = (float)lua_tonumber(L, 2);
	tv->Width = width;
	return 0;
}

luaL_Reg MT_UI_TEXTVIEW[] = {
	{ "SetWidth", ui_textview_set_width},
	{ "SetText",ui_textview_set_text},
	{ "SetPos",ui_textview_set_pos},
	{ "SetColor",ui_textview_set_color},
	{ "SetBGColor",ui_textview_set_bg_color},
	{ "SetTextSize",ui_textview_set_text_size},
	{NULL,NULL}
};

int ui_textview_create(lua_State* L)
{
	lua_push_pointer(L, new UITextView());
	if (luaL_newmetatable(L, "MT_UI_TEXTVIEW")) {
		luaL_setfuncs(L, MT_UI_TEXTVIEW, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}
void ui_renderer_clear(){
	UIRenderer::GetInstance()->Clear();
}

void npc_dialog_show(bool show,const char* txt){
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	dlg->SetText(txt);
	dlg->SetVisible(show);
}

void npc_dialog_set_xy(int x, int y) {
	auto* dlg = UIRenderer::GetInstance()->GetDialog();
	dlg->X = (float)x;
	dlg->Y = (float)y;
}

void luaopen_ui_renderer(lua_State* L)
{
	script_system_register_luac_function(L, ne_imageview_create);
	script_system_register_luac_function(L, ui_textview_create);
	script_system_register_luac_function(L, ui_renderer_add_to_draw);
	script_system_register_function(L, ui_renderer_clear);
	script_system_register_function(L, npc_dialog_show);
	script_system_register_function(L, npc_dialog_set_xy);
}
