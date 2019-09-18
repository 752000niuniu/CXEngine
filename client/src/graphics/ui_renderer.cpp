#include "ui_renderer.h"
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "window.h"
#include "file_system.h"
#include "input_manager.h"


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


NVGcontext* vg = NULL;
UIRenderer::UIRenderer()
{
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return;
	}

	nvgCreateFont(vg, "MSYH", FileSystem::GetFontPath("msyh.ttf").c_str());
	

}

UIRenderer::~UIRenderer()
{
	nvgDeleteGL3(vg);
}

void UIRenderer::Update()
{

}

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

	drawLines(vg, 120, height - 50, 600, 50, glfwGetTime());
	nvgEndFrame(vg);

	
}

