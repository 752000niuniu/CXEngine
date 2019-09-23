#include "ui_renderer.h"
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "window.h"
#include "file_system.h"
#include "input_manager.h"
#include "script_system.h"
#include "animation/sprite.h"


void drawParagraph(NVGcontext* vg, float x, float y, float width, float height, float mx, float my)
{
	NVGtextRow rows[3];
	NVGglyphPosition glyphs[100];
	const char* text = u8"往事蒙尘在我眉睫之间，\n方今重回到江南旧院。\n残朽了，岁月刻的牌匾，\n叩开回忆之门一问尘缘。\n我乘乌篷船翩然过桥亭，\n与清明桥上嫣然那个你，\n不经意，相看成了风景。\n我蘸酒写诗而你误入诗句。\n我轻弹古筝歌遍，\n方知断了的琴弦，再也唱不出思念。\n你刺绣化梦的蝶，\n后觉断了的红线，再也绣不出情缘。\n执手昨日疏影西窗前，灯花落尽檐外月一剪，\n棋局与人生，哪个更多劫？\n孑然今宵微雨断桥边，往事焚灰你香冢长眠，\n从此处处烟波，都似你眉眼。\n多年后我又乘船过江南，\n可清明桥上再无你倩影，\n未留意，眷顾成了曾经。\n一道古镇清风长叹了半声。\n玉笛余音向天阙，流年也沉默封缄，\n为你静敛岁月吊唁。\n黛瓦青砖雨不绝，我蒙霜冷彻心间，\n为你倾尽悼念。\n或许怨天命旁观冷眼，或许问天命总妒良缘，\n说莫失莫忘，无常总上演。\n天涯一隔两端有多远，怎敌一隔阴阳两相望，\n从此处处苍翠，都似你裙边。\n忆江南山悠然水悠然，你眸凝万水眉黛千山，\n待百年与你，共长眠江南。\n忆江南船依然桥依然，与你将前缘再续编撰：\n若有三生一世，再遇你江南。";
	const char* start;
	const char* end;
	int nrows, i, nglyphs, j, lnum = 0;
	float lineh;
	float caretx, px;
	float bounds[4];
	float a;
	float gx, gy;
	int gutter = 0;
	NVG_NOTUSED(height);

	nvgSave(vg);

	nvgFontSize(vg, 18.f);
	nvgFontFace(vg, "MSYH");
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgTextMetrics(vg, NULL, NULL, &lineh);

	// The text break API can be used to fill a large buffer of rows,
	// or to iterate over the text just few lines (or just one) at a time.
	// The "next" variable of the last returned item tells where to continue.
	start = text;
	end = text + strlen(text);
	while ((nrows = nvgTextBreakLines(vg, start, end, width, rows, 3))) {
		for (i = 0; i < nrows; i++) {
			NVGtextRow* row = &rows[i];
			int hit = mx > x && mx < (x + width) && my >= y && my < (y + lineh);

			nvgBeginPath(vg);
			nvgFillColor(vg, nvgRGBA(255, 255, 255, hit ? 64 : 16));
			nvgRect(vg, x, y, row->width, lineh);
			nvgFill(vg);

			nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
			nvgText(vg, x, y, row->start, row->end);

			if (hit) {
				caretx = (mx < x + row->width / 2) ? x : x + row->width;
				px = x;
				nglyphs = nvgTextGlyphPositions(vg, x, y, row->start, row->end, glyphs, 100);
				for (j = 0; j < nglyphs; j++) {
					float x0 = glyphs[j].x;
					float x1 = (j + 1 < nglyphs) ? glyphs[j + 1].x : x + row->width;
					float gx = x0 * 0.3f + x1 * 0.7f;
					if (mx >= px && mx < gx)
						caretx = glyphs[j].x;
					px = gx;
				}
				nvgBeginPath(vg);
				nvgFillColor(vg, nvgRGBA(255, 192, 0, 255));
				nvgRect(vg, caretx, y, 1, lineh);
				nvgFill(vg);

				gutter = lnum + 1;
				gx = x - 10;
				gy = y + lineh / 2;
			}
			lnum++;
			y += lineh;
		}
		// Keep going...
		start = rows[nrows - 1].next;
	}

	if (gutter) {
		char txt[16];
		snprintf(txt, sizeof(txt), "%d", gutter);
		nvgFontSize(vg, 13.0f);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		nvgTextBounds(vg, gx, gy, txt, NULL, bounds);

		nvgBeginPath(vg);
		nvgFillColor(vg, nvgRGBA(255, 192, 0, 255));
		nvgRoundedRect(vg, (int)bounds[0] - 4, (int)bounds[1] - 2, (int)(bounds[2] - bounds[0]) + 8, (int)(bounds[3] - bounds[1]) + 4, ((int)(bounds[3] - bounds[1]) + 4) / 2 - 1);
		nvgFill(vg);

		nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
		nvgText(vg, gx, gy, txt, NULL);
	}

	y += 20.0f;

	nvgFontSize(vg, 13.0f);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgTextLineHeight(vg, 1.2f);

	nvgTextBoxBounds(vg, x, y, 150, "Hover your mouse over the text to see calculated caret position.", NULL, bounds);

	// Fade the tooltip out when close to it.
	gx = fabsf((mx - (bounds[0] + bounds[2]) * 0.5f) / (bounds[0] - bounds[2]));
	gy = fabsf((my - (bounds[1] + bounds[3]) * 0.5f) / (bounds[1] - bounds[3]));
	a = std::max(gx, gy) - 0.5f;
	a = glm::clamp(a, 0.0f, 1.0f);
	nvgGlobalAlpha(vg, a);

	nvgBeginPath(vg);
	nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
	nvgRoundedRect(vg, bounds[0] - 2, bounds[1] - 2, (int)(bounds[2] - bounds[0]) + 4, (int)(bounds[3] - bounds[1]) + 4, 3);
	px = (int)((bounds[2] + bounds[0]) / 2);
	nvgMoveTo(vg, px, bounds[1] - 10);
	nvgLineTo(vg, px + 7, bounds[1] + 1);
	nvgLineTo(vg, px - 7, bounds[1] + 1);
	nvgFill(vg);

	nvgFillColor(vg, nvgRGBA(0, 0, 0, 220));
	nvgTextBox(vg, x, y, 150, "Hover your mouse over the text to see calculated caret position.", NULL);

	nvgRestore(vg);
}


void drawLines(NVGcontext* vg, float x, float y, float w, float h, float t)
{
	int i, j;
	float pad = 5.0f, s = w / 9.0f - pad * 2;
	float pts[4 * 2], fx, fy;
	int joins[3] = { NVG_MITER, NVG_ROUND, NVG_BEVEL };
	int caps[3] = { NVG_BUTT, NVG_ROUND, NVG_SQUARE };
	NVG_NOTUSED(h);

	nvgSave(vg);
	pts[0] = -s * 0.25f + cosf(t * 0.3f) * s * 0.5f;
	pts[1] = sinf(t * 0.3f) * s * 0.5f;
	pts[2] = -s * 0.25;
	pts[3] = 0;
	pts[4] = s * 0.25f;
	pts[5] = 0;
	pts[6] = s * 0.25f + cosf(-t * 0.3f) * s * 0.5f;
	pts[7] = sinf(-t * 0.3f) * s * 0.5f;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			fx = x + s * 0.5f + (i * 3 + j) / 9.0f * w + pad;
			fy = y - s * 0.5f + pad;

			nvgLineCap(vg, caps[i]);
			nvgLineJoin(vg, joins[j]);

			nvgStrokeWidth(vg, s * 0.3f);
			nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 160));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx + pts[0], fy + pts[1]);
			nvgLineTo(vg, fx + pts[2], fy + pts[3]);
			nvgLineTo(vg, fx + pts[4], fy + pts[5]);
			nvgLineTo(vg, fx + pts[6], fy + pts[7]);
			nvgStroke(vg);

			nvgLineCap(vg, NVG_BUTT);
			nvgLineJoin(vg, NVG_BEVEL);

			nvgStrokeWidth(vg, 1.0f);
			nvgStrokeColor(vg, nvgRGBA(0, 192, 255, 255));
			nvgBeginPath(vg);
			nvgMoveTo(vg, fx + pts[0], fy + pts[1]);
			nvgLineTo(vg, fx + pts[2], fy + pts[3]);
			nvgLineTo(vg, fx + pts[4], fy + pts[5]);
			nvgLineTo(vg, fx + pts[6], fy + pts[7]);
			nvgStroke(vg);
		}
	}
	nvgRestore(vg);
}


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
		printf("Could not init nanovg.\n");
		return;
	}

	int res = 0;
	res =nvgCreateFont(vg, "MSYH", FileSystem::GetFontPath("msyh.ttf").c_str());
	res =nvgCreateFont(vg, "MSHT", FileSystem::GetFontPath("msht.ttf").c_str());
	res= nvgCreateFont(vg, "SIMSUN", FileSystem::GetFontPath("simsun.ttc").c_str());
}

UIRenderer::~UIRenderer()
{
	nvgDeleteGL3(vg);
}

void UIRenderer::Update()
{

}

void nano_render_text(int x, int y, int width, int size, int textcolor, int textbgcolor, const char* font, const char* text);
void UIRenderer::Draw()
{
	int width = WINDOW_INSTANCE->GetWidth();
	int height = WINDOW_INSTANCE->GetHeight();
	Pos mpos = INPUT_MANAGER_INSTANCE->GetMousePos();
	int mx = mpos.x;
	int my = mpos.y;
	
	glClear(GL_STENCIL_BUFFER_BIT);
	nvgBeginFrame(vg, width, height, width* 1.0f / height);

//	drawParagraph(vg, width - 450, 50, 150, 100, mx, my);
//	nano_render_text(0, 0, 50, 24,0xffffffff,0xff00ffff,  "MSHT", u8"I need a girl!");
	
	
	//script_system_call_function(script_system_get_luastate(), "on_ui_renderer_draw");
	for (auto* obj : m_Objects){
		obj->Draw();
	}

	//drawLines(vg, 120, height - 50, 600, 50, glfwGetTime());
	nvgEndFrame(vg);
}

void UIRenderer::AddToDraw(UIObject* obj)
{
	m_Objects.push_back(obj);
}

void nano_render_text(int x, int y, int width, int size, int textcolor, int textbgcolor, const char* font, const char* text)
{
#define break_color_rgba(c)  nvgRGBA((c& 0xff), (c& 0xff00) >> 8, (c& 0xff0000) >> 16, (c& 0xff000000) >> 24)
	nvgSave(vg);
	float lineh;
	uint32_t color = (uint32_t)textcolor;
	uint32_t bgcolor = (uint32_t)textbgcolor;

	nvgFontSize(vg, size);
	nvgFontFace(vg, font);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgTextMetrics(vg, NULL, NULL, &lineh);

	const char* start;
	const char* end;
	start = text;
	end = text + strlen(text);

	NVGtextRow rows[3];
	int nrows, lnum = 0;
	while ((nrows = nvgTextBreakLines(vg, start, end, width, rows, 3))) {
		for (int i = 0; i < nrows; i++) {
			NVGtextRow* row = &rows[i];

			nvgBeginPath(vg);
			nvgFillColor(vg, break_color_rgba(bgcolor));
			nvgRect(vg, x, y, row->width, lineh);
			nvgFill(vg);


			nvgFillColor(vg, break_color_rgba(color));
			nvgText(vg, x, y, row->start, row->end);

			lnum++;
			y += lineh;
		}
		start = rows[nrows - 1].next;
	}

	nvgRestore(vg);
#undef break_color_rgba
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
		imageid = nvgCreateImageRGBA(vg, frame.Width, frame.Height, NVG_IMAGE_NEAREST, (unsigned char*)frame.Src.data());
		s_ImageCache.insert({ id,imageid });
	}
	else {
		imageid = it->second;
	}
	nvgSave(vg);
	float left = m_pBS->Pos.x - frame.KeyX;
	float top = m_pBS->Pos.y - frame.KeyY;
	float w = m_pBS->Width;
	float h = m_pBS->Height;
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
	Font("SIMSUN"),
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

	if(Width!=0){
		NVGtextRow rows[3];
		int nrows, lnum = 0;
		while ((nrows = nvgTextBreakLines(vg, start, end, Width, rows, 3))) {
			for (int i = 0; i < nrows; i++) {
				NVGtextRow* row = &rows[i];

				nvgBeginPath(vg);
				nvgFillColor(vg, BGColor);
				nvgRect(vg, X, Y, Width, lineh);
				nvgFill(vg);


				nvgFillColor(vg, Color);
				nvgText(vg, X, Y, row->start, row->end);

				lnum++;
				Y += lineh;
			}
			start = rows[nrows - 1].next;
		}

	}else{
		nvgFillColor(vg, Color);
		nvgText(vg, X, Y, Text.c_str(), NULL);
	}
	
	nvgRestore(vg);
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
luaL_Reg MT_NE_IMAGEVIEW[] = {
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


void luaopen_ui_renderer(lua_State* L)
{
	
	script_system_register_luac_function(L, ne_imageview_create);
	script_system_register_function(L, nano_render_text);
	script_system_register_luac_function(L, ui_renderer_add_to_draw);
}
