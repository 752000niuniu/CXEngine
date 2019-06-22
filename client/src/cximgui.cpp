//ImGuiContext*  CreateContext(ImFontAtlas* shared_font_atlas);
//arg1	ImFontAtlas*	shared_font_atlas	=	 NULL
//[CreateContext] unsupported arg type!!!

//void  DestroyContext(ImGuiContext* ctx);
//arg1	ImGuiContext*	ctx	=	 NULL
//[DestroyContext] unsupported arg type!!!

//ImGuiContext*  GetCurrentContext();
int cximgui_GetCurrentContext(lua_State* L) {
	ImGuiContext* __ret__ = ImGui::GetCurrentContext();
	return 0;
};

//void  SetCurrentContext(ImGuiContext* ctx);
//arg1	ImGuiContext*	ctx	=	
//[SetCurrentContext] unsupported arg type!!!

//bool  DebugCheckVersionAndDataLayout(const char* version_str,size_t sz_io,size_t sz_style,size_t sz_vec2,size_t sz_vec4,size_t sz_drawvert);
//arg1	const char*	version_str	=	
//arg2	size_t	sz_io	=	
//arg3	size_t	sz_style	=	
//arg4	size_t	sz_vec2	=	
//arg5	size_t	sz_vec4	=	
//arg6	size_t	sz_drawvert	=	
int cximgui_DebugCheckVersionAndDataLayout_6_siiiii(lua_State* L) {
	int __argi__ = 0;
	const char* version_str = lua_tostring(L, __argi__++);
	size_t sz_io = (size_t)lua_tointeger(L, __argi__++);
	size_t sz_style = (size_t)lua_tointeger(L, __argi__++);
	size_t sz_vec2 = (size_t)lua_tointeger(L, __argi__++);
	size_t sz_vec4 = (size_t)lua_tointeger(L, __argi__++);
	size_t sz_drawvert = (size_t)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::DebugCheckVersionAndDataLayout(version_str, sz_io, sz_style, sz_vec2, sz_vec4, sz_drawvert);
	lua_pushboolean(L, __ret__);
	return 1;
};

//ImGuiIO&  GetIO();
int cximgui_GetIO(lua_State* L) {
	ImGuiIO& __ret__ = ImGui::GetIO();
	return 0;
};

//ImGuiStyle&  GetStyle();
int cximgui_GetStyle(lua_State* L) {
	ImGuiStyle& __ret__ = ImGui::GetStyle();
	return 0;
};

//void  NewFrame();
int cximgui_NewFrame(lua_State* L) {
	ImGui::NewFrame();
	return 0;
};

//void  EndFrame();
int cximgui_EndFrame(lua_State* L) {
	ImGui::EndFrame();
	return 0;
};

//void  Render();
int cximgui_Render(lua_State* L) {
	ImGui::Render();
	return 0;
};

//ImDrawData*  GetDrawData();
int cximgui_GetDrawData(lua_State* L) {
	ImDrawData* __ret__ = ImGui::GetDrawData();
	return 0;
};

//void  ShowDemoWindow(bool* p_open);
//arg1	bool*	p_open	=	 NULL
int cximgui_ShowDemoWindow_1_bp(lua_State* L) {
	int __argi__ = 0;
	bool p_open = lua_toboolean(L, __argi__++);
	ImGui::ShowDemoWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
};

//void  ShowMetricsWindow(bool* p_open);
//arg1	bool*	p_open	=	 NULL
int cximgui_ShowMetricsWindow_1_bp(lua_State* L) {
	int __argi__ = 0;
	bool p_open = lua_toboolean(L, __argi__++);
	ImGui::ShowMetricsWindow(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
};

//void  ShowStyleEditor(ImGuiStyle* ref);
//arg1	ImGuiStyle*	ref	=	 NULL
//[ShowStyleEditor] unsupported arg type!!!

//bool  ShowStyleSelector(const char* label);
//arg1	const char*	label	=	
int cximgui_ShowStyleSelector_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::ShowStyleSelector(label);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  ShowFontSelector(const char* label);
//arg1	const char*	label	=	
int cximgui_ShowFontSelector_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImGui::ShowFontSelector(label);
	return 0;
};

//void  ShowUserGuide();
int cximgui_ShowUserGuide(lua_State* L) {
	ImGui::ShowUserGuide();
	return 0;
};

//const char*  GetVersion();
int cximgui_GetVersion(lua_State* L) {
	const char* __ret__ = ImGui::GetVersion();
	lua_pushstring(L, __ret__);
	return 1;
};

//void  StyleColorsDark(ImGuiStyle* dst);
//arg1	ImGuiStyle*	dst	=	 NULL
//[StyleColorsDark] unsupported arg type!!!

//void  StyleColorsClassic(ImGuiStyle* dst);
//arg1	ImGuiStyle*	dst	=	 NULL
//[StyleColorsClassic] unsupported arg type!!!

//void  StyleColorsLight(ImGuiStyle* dst);
//arg1	ImGuiStyle*	dst	=	 NULL
//[StyleColorsLight] unsupported arg type!!!

//bool  Begin(const char* name,bool* p_open,ImGuiWindowFlags flags);
//arg1	const char*	name	=	
//arg2	bool*	p_open	=	 NULL
//arg3	ImGuiWindowFlags	flags	=	 0
int cximgui_Begin_3_sbpi(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	bool p_open = lua_toboolean(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::Begin(name, &p_open, flags);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_open);
	return 2;
};

//void  End();
int cximgui_End(lua_State* L) {
	ImGui::End();
	return 0;
};

//bool  BeginChild(const char* str_id,const ImVec2& size,bool border,ImGuiWindowFlags flags);
//arg1	const char*	str_id	=	
//arg2	const ImVec2&	size	=	 ImVec2 @1
//arg3	bool	border	=	 false
//arg4	ImGuiWindowFlags	flags	=	 0
int cximgui_BeginChild_4_sv2bi(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool border = lua_toboolean(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginChild(str_id, size, border, flags);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  BeginChild(ImGuiID id,const ImVec2& size,bool border,ImGuiWindowFlags flags);
//arg1	ImGuiID	id	=	
//arg2	const ImVec2&	size	=	 ImVec2 @1
//arg3	bool	border	=	 false
//arg4	ImGuiWindowFlags	flags	=	 0
int cximgui_BeginChild_4_iv2bi(lua_State* L) {
	int __argi__ = 0;
	ImGuiID id = (ImGuiID)lua_tointeger(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool border = lua_toboolean(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginChild(id, size, border, flags);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//void  EndChild();
int cximgui_EndChild(lua_State* L) {
	ImGui::EndChild();
	return 0;
};

//bool  IsWindowAppearing();
int cximgui_IsWindowAppearing(lua_State* L) {
	bool __ret__ = ImGui::IsWindowAppearing();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsWindowCollapsed();
int cximgui_IsWindowCollapsed(lua_State* L) {
	bool __ret__ = ImGui::IsWindowCollapsed();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsWindowFocused(ImGuiFocusedFlags flags);
//arg1	ImGuiFocusedFlags	flags	=	0
int cximgui_IsWindowFocused_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiFocusedFlags flags = (ImGuiFocusedFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsWindowFocused(flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsWindowHovered(ImGuiHoveredFlags flags);
//arg1	ImGuiHoveredFlags	flags	=	0
int cximgui_IsWindowHovered_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiHoveredFlags flags = (ImGuiHoveredFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsWindowHovered(flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//ImDrawList*  GetWindowDrawList();
int cximgui_GetWindowDrawList(lua_State* L) {
	ImDrawList* __ret__ = ImGui::GetWindowDrawList();
	return 0;
};

//float  GetWindowDpiScale();
int cximgui_GetWindowDpiScale(lua_State* L) {
	float __ret__ = ImGui::GetWindowDpiScale();
	lua_pushnumber(L, __ret__);
	return 1;
};

//ImGuiViewport*  GetWindowViewport();
int cximgui_GetWindowViewport(lua_State* L) {
	ImGuiViewport* __ret__ = ImGui::GetWindowViewport();
	return 0;
};

//ImVec2  GetWindowPos();
int cximgui_GetWindowPos(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetWindowPos();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetWindowSize();
int cximgui_GetWindowSize(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetWindowSize();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//float  GetWindowWidth();
int cximgui_GetWindowWidth(lua_State* L) {
	float __ret__ = ImGui::GetWindowWidth();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetWindowHeight();
int cximgui_GetWindowHeight(lua_State* L) {
	float __ret__ = ImGui::GetWindowHeight();
	lua_pushnumber(L, __ret__);
	return 1;
};

//ImVec2  GetContentRegionMax();
int cximgui_GetContentRegionMax(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetContentRegionMax();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetContentRegionAvail();
int cximgui_GetContentRegionAvail(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetContentRegionAvail();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//float  GetContentRegionAvailWidth();
int cximgui_GetContentRegionAvailWidth(lua_State* L) {
	float __ret__ = ImGui::GetContentRegionAvailWidth();
	lua_pushnumber(L, __ret__);
	return 1;
};

//ImVec2  GetWindowContentRegionMin();
int cximgui_GetWindowContentRegionMin(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetWindowContentRegionMin();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetWindowContentRegionMax();
int cximgui_GetWindowContentRegionMax(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetWindowContentRegionMax();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//float  GetWindowContentRegionWidth();
int cximgui_GetWindowContentRegionWidth(lua_State* L) {
	float __ret__ = ImGui::GetWindowContentRegionWidth();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetNextWindowPos(const ImVec2& pos,ImGuiCond cond,const ImVec2& pivot);
//arg1	const ImVec2&	pos	=	
//arg2	ImGuiCond	cond	=	 0
//arg3	const ImVec2&	pivot	=	 ImVec2 @1
int cximgui_SetNextWindowPos_3_v2iv2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 pos;
	pos.x = (float)lua_tonumber(L, __argi__++);
	pos.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImVec2 pivot;
	pivot.x = (float)lua_tonumber(L, __argi__++);
	pivot.y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetNextWindowPos(pos, cond, pivot);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pivot.x);
	lua_pushnumber(L, pivot.y);
	return 4;
};

//void  SetNextWindowSize(const ImVec2& size,ImGuiCond cond);
//arg1	const ImVec2&	size	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetNextWindowSize_2_v2i(lua_State* L) {
	int __argi__ = 0;
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetNextWindowSize(size, cond);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
};

//void  SetNextWindowSizeConstraints(const ImVec2& size_min,const ImVec2& size_max,ImGuiSizeCallback custom_callback,void* custom_callback_data);
//arg1	const ImVec2&	size_min	=	
//arg2	const ImVec2&	size_max	=	
//arg3	ImGuiSizeCallback	custom_callback	=	 NULL
//arg4	void*	custom_callback_data	=	 NULL
//UnSupported SetNextWindowSizeConstraints
//void  SetNextWindowContentSize(const ImVec2& size);
//arg1	const ImVec2&	size	=	
int cximgui_SetNextWindowContentSize_1_v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetNextWindowContentSize(size);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
};

//void  SetNextWindowCollapsed(bool collapsed,ImGuiCond cond);
//arg1	bool	collapsed	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetNextWindowCollapsed_2_bi(lua_State* L) {
	int __argi__ = 0;
	bool collapsed = lua_toboolean(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetNextWindowCollapsed(collapsed, cond);
	return 0;
};

//void  SetNextWindowFocus();
int cximgui_SetNextWindowFocus(lua_State* L) {
	ImGui::SetNextWindowFocus();
	return 0;
};

//void  SetNextWindowBgAlpha(float alpha);
//arg1	float	alpha	=	
int cximgui_SetNextWindowBgAlpha_1_n(lua_State* L) {
	int __argi__ = 0;
	float alpha = (float)lua_tonumber(L, __argi__++);
	ImGui::SetNextWindowBgAlpha(alpha);
	return 0;
};

//void  SetNextWindowViewport(ImGuiID viewport_id);
//arg1	ImGuiID	viewport_id	=	
int cximgui_SetNextWindowViewport_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiID viewport_id = (ImGuiID)lua_tointeger(L, __argi__++);
	ImGui::SetNextWindowViewport(viewport_id);
	return 0;
};

//void  SetWindowPos(const ImVec2& pos,ImGuiCond cond);
//arg1	const ImVec2&	pos	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetWindowPos_2_v2i(lua_State* L) {
	int __argi__ = 0;
	ImVec2 pos;
	pos.x = (float)lua_tonumber(L, __argi__++);
	pos.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowPos(pos, cond);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
};

//void  SetWindowSize(const ImVec2& size,ImGuiCond cond);
//arg1	const ImVec2&	size	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetWindowSize_2_v2i(lua_State* L) {
	int __argi__ = 0;
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowSize(size, cond);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
};

//void  SetWindowCollapsed(bool collapsed,ImGuiCond cond);
//arg1	bool	collapsed	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetWindowCollapsed_2_bi(lua_State* L) {
	int __argi__ = 0;
	bool collapsed = lua_toboolean(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowCollapsed(collapsed, cond);
	return 0;
};

//void  SetWindowFocus();
int cximgui_SetWindowFocus(lua_State* L) {
	ImGui::SetWindowFocus();
	return 0;
};

//void  SetWindowFontScale(float scale);
//arg1	float	scale	=	
int cximgui_SetWindowFontScale_1_n(lua_State* L) {
	int __argi__ = 0;
	float scale = (float)lua_tonumber(L, __argi__++);
	ImGui::SetWindowFontScale(scale);
	return 0;
};

//void  SetWindowPos(const char* name,const ImVec2& pos,ImGuiCond cond);
//arg1	const char*	name	=	
//arg2	const ImVec2&	pos	=	
//arg3	ImGuiCond	cond	=	 0
int cximgui_SetWindowPos_3_sv2i(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	ImVec2 pos;
	pos.x = (float)lua_tonumber(L, __argi__++);
	pos.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowPos(name, pos, cond);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
};

//void  SetWindowSize(const char* name,const ImVec2& size,ImGuiCond cond);
//arg1	const char*	name	=	
//arg2	const ImVec2&	size	=	
//arg3	ImGuiCond	cond	=	 0
int cximgui_SetWindowSize_3_sv2i(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowSize(name, size, cond);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
};

//void  SetWindowCollapsed(const char* name,bool collapsed,ImGuiCond cond);
//arg1	const char*	name	=	
//arg2	bool	collapsed	=	
//arg3	ImGuiCond	cond	=	 0
int cximgui_SetWindowCollapsed_3_sbi(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	bool collapsed = lua_toboolean(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetWindowCollapsed(name, collapsed, cond);
	return 0;
};

//void  SetWindowFocus(const char* name);
//arg1	const char*	name	=	
int cximgui_SetWindowFocus_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	ImGui::SetWindowFocus(name);
	return 0;
};

//float  GetScrollX();
int cximgui_GetScrollX(lua_State* L) {
	float __ret__ = ImGui::GetScrollX();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetScrollY();
int cximgui_GetScrollY(lua_State* L) {
	float __ret__ = ImGui::GetScrollY();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetScrollMaxX();
int cximgui_GetScrollMaxX(lua_State* L) {
	float __ret__ = ImGui::GetScrollMaxX();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetScrollMaxY();
int cximgui_GetScrollMaxY(lua_State* L) {
	float __ret__ = ImGui::GetScrollMaxY();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetScrollX(float scroll_x);
//arg1	float	scroll_x	=	
int cximgui_SetScrollX_1_n(lua_State* L) {
	int __argi__ = 0;
	float scroll_x = (float)lua_tonumber(L, __argi__++);
	ImGui::SetScrollX(scroll_x);
	return 0;
};

//void  SetScrollY(float scroll_y);
//arg1	float	scroll_y	=	
int cximgui_SetScrollY_1_n(lua_State* L) {
	int __argi__ = 0;
	float scroll_y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetScrollY(scroll_y);
	return 0;
};

//void  SetScrollHereY(float center_y_ratio);
//arg1	float	center_y_ratio	=	 0.5f
int cximgui_SetScrollHereY_1_n(lua_State* L) {
	int __argi__ = 0;
	float center_y_ratio = (float)lua_tonumber(L, __argi__++);
	ImGui::SetScrollHereY(center_y_ratio);
	return 0;
};

//void  SetScrollFromPosY(float pos_y,float center_y_ratio);
//arg1	float	pos_y	=	
//arg2	float	center_y_ratio	=	 0.5f
int cximgui_SetScrollFromPosY_2_nn(lua_State* L) {
	int __argi__ = 0;
	float pos_y = (float)lua_tonumber(L, __argi__++);
	float center_y_ratio = (float)lua_tonumber(L, __argi__++);
	ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
	return 0;
};

//void  PushFont(ImFont* font);
//arg1	ImFont*	font	=	
//[PushFont] unsupported arg type!!!

//void  PopFont();
int cximgui_PopFont(lua_State* L) {
	ImGui::PopFont();
	return 0;
};

//void  PushStyleColor(ImGuiCol idx,ImU32 col);
//arg1	ImGuiCol	idx	=	
//arg2	ImU32	col	=	
int cximgui_PushStyleColor_2_ii(lua_State* L) {
	int __argi__ = 0;
	ImGuiCol idx = (ImGuiCol)lua_tointeger(L, __argi__++);
	ImU32 col = (ImU32)lua_tointeger(L, __argi__++);
	ImGui::PushStyleColor(idx, col);
	return 0;
};

//void  PushStyleColor(ImGuiCol idx,const ImVec4& col);
//arg1	ImGuiCol	idx	=	
//arg2	const ImVec4&	col	=	
int cximgui_PushStyleColor_2_iv4(lua_State* L) {
	int __argi__ = 0;
	ImGuiCol idx = (ImGuiCol)lua_tointeger(L, __argi__++);
	ImVec4 col;
	col.x = (float)lua_tonumber(L, __argi__++);
	col.y = (float)lua_tonumber(L, __argi__++);
	col.z = (float)lua_tonumber(L, __argi__++);
	col.w = (float)lua_tonumber(L, __argi__++);
	ImGui::PushStyleColor(idx, col);
	lua_pushnumber(L, col.x);
	lua_pushnumber(L, col.y);
	lua_pushnumber(L, col.z);
	lua_pushnumber(L, col.w);
	return 4;
};

//void  PopStyleColor(int count);
//arg1	int	count	=	 1
int cximgui_PopStyleColor_1_i(lua_State* L) {
	int __argi__ = 0;
	int count = (int)lua_tointeger(L, __argi__++);
	ImGui::PopStyleColor(count);
	return 0;
};

//void  PushStyleVar(ImGuiStyleVar idx,float val);
//arg1	ImGuiStyleVar	idx	=	
//arg2	float	val	=	
int cximgui_PushStyleVar_2_in(lua_State* L) {
	int __argi__ = 0;
	ImGuiStyleVar idx = (ImGuiStyleVar)lua_tointeger(L, __argi__++);
	float val = (float)lua_tonumber(L, __argi__++);
	ImGui::PushStyleVar(idx, val);
	return 0;
};

//void  PushStyleVar(ImGuiStyleVar idx,const ImVec2& val);
//arg1	ImGuiStyleVar	idx	=	
//arg2	const ImVec2&	val	=	
int cximgui_PushStyleVar_2_iv2(lua_State* L) {
	int __argi__ = 0;
	ImGuiStyleVar idx = (ImGuiStyleVar)lua_tointeger(L, __argi__++);
	ImVec2 val;
	val.x = (float)lua_tonumber(L, __argi__++);
	val.y = (float)lua_tonumber(L, __argi__++);
	ImGui::PushStyleVar(idx, val);
	lua_pushnumber(L, val.x);
	lua_pushnumber(L, val.y);
	return 2;
};

//void  PopStyleVar(int count);
//arg1	int	count	=	 1
int cximgui_PopStyleVar_1_i(lua_State* L) {
	int __argi__ = 0;
	int count = (int)lua_tointeger(L, __argi__++);
	ImGui::PopStyleVar(count);
	return 0;
};

//const ImVec4&  GetStyleColorVec4(ImGuiCol idx);
//arg1	ImGuiCol	idx	=	
int cximgui_GetStyleColorVec4_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiCol idx = (ImGuiCol)lua_tointeger(L, __argi__++);
	const ImVec4& __ret__ = ImGui::GetStyleColorVec4(idx);
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	lua_pushnumber(L, __ret__.z);
	lua_pushnumber(L, __ret__.w);
	return 4;
};

//ImFont*  GetFont();
int cximgui_GetFont(lua_State* L) {
	ImFont* __ret__ = ImGui::GetFont();
	return 0;
};

//float  GetFontSize();
int cximgui_GetFontSize(lua_State* L) {
	float __ret__ = ImGui::GetFontSize();
	lua_pushnumber(L, __ret__);
	return 1;
};

//ImVec2  GetFontTexUvWhitePixel();
int cximgui_GetFontTexUvWhitePixel(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetFontTexUvWhitePixel();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImU32  GetColorU32(ImGuiCol idx,float alpha_mul);
//arg1	ImGuiCol	idx	=	
//arg2	float	alpha_mul	=	 1.0f
int cximgui_GetColorU32_2_in(lua_State* L) {
	int __argi__ = 0;
	ImGuiCol idx = (ImGuiCol)lua_tointeger(L, __argi__++);
	float alpha_mul = (float)lua_tonumber(L, __argi__++);
	ImU32 __ret__ = ImGui::GetColorU32(idx, alpha_mul);
	lua_pushinteger(L, __ret__);
	return 1;
};

//ImU32  GetColorU32(const ImVec4& col);
//arg1	const ImVec4&	col	=	
int cximgui_GetColorU32_1_v4(lua_State* L) {
	int __argi__ = 0;
	ImVec4 col;
	col.x = (float)lua_tonumber(L, __argi__++);
	col.y = (float)lua_tonumber(L, __argi__++);
	col.z = (float)lua_tonumber(L, __argi__++);
	col.w = (float)lua_tonumber(L, __argi__++);
	ImU32 __ret__ = ImGui::GetColorU32(col);
	lua_pushinteger(L, __ret__);
	lua_pushnumber(L, col.x);
	lua_pushnumber(L, col.y);
	lua_pushnumber(L, col.z);
	lua_pushnumber(L, col.w);
	return 5;
};

//ImU32  GetColorU32(ImU32 col);
//arg1	ImU32	col	=	
int cximgui_GetColorU32_1_i(lua_State* L) {
	int __argi__ = 0;
	ImU32 col = (ImU32)lua_tointeger(L, __argi__++);
	ImU32 __ret__ = ImGui::GetColorU32(col);
	lua_pushinteger(L, __ret__);
	return 1;
};

//void  PushItemWidth(float item_width);
//arg1	float	item_width	=	
int cximgui_PushItemWidth_1_n(lua_State* L) {
	int __argi__ = 0;
	float item_width = (float)lua_tonumber(L, __argi__++);
	ImGui::PushItemWidth(item_width);
	return 0;
};

//void  PopItemWidth();
int cximgui_PopItemWidth(lua_State* L) {
	ImGui::PopItemWidth();
	return 0;
};

//float  CalcItemWidth();
int cximgui_CalcItemWidth(lua_State* L) {
	float __ret__ = ImGui::CalcItemWidth();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  PushTextWrapPos(float wrap_pos_x);
//arg1	float	wrap_pos_x	=	 0.0f
int cximgui_PushTextWrapPos_1_n(lua_State* L) {
	int __argi__ = 0;
	float wrap_pos_x = (float)lua_tonumber(L, __argi__++);
	ImGui::PushTextWrapPos(wrap_pos_x);
	return 0;
};

//void  PopTextWrapPos();
int cximgui_PopTextWrapPos(lua_State* L) {
	ImGui::PopTextWrapPos();
	return 0;
};

//void  PushAllowKeyboardFocus(bool allow_keyboard_focus);
//arg1	bool	allow_keyboard_focus	=	
int cximgui_PushAllowKeyboardFocus_1_b(lua_State* L) {
	int __argi__ = 0;
	bool allow_keyboard_focus = lua_toboolean(L, __argi__++);
	ImGui::PushAllowKeyboardFocus(allow_keyboard_focus);
	return 0;
};

//void  PopAllowKeyboardFocus();
int cximgui_PopAllowKeyboardFocus(lua_State* L) {
	ImGui::PopAllowKeyboardFocus();
	return 0;
};

//void  PushButtonRepeat(bool repeat);
//arg1	bool	repeat	=	
int cximgui_PushButtonRepeat_1_b(lua_State* L) {
	int __argi__ = 0;
	bool repeat = lua_toboolean(L, __argi__++);
	ImGui::PushButtonRepeat(repeat);
	return 0;
};

//void  PopButtonRepeat();
int cximgui_PopButtonRepeat(lua_State* L) {
	ImGui::PopButtonRepeat();
	return 0;
};

//void  Separator();
int cximgui_Separator(lua_State* L) {
	ImGui::Separator();
	return 0;
};

//void  SameLine(float pos_x,float spacing_w);
//arg1	float	pos_x	=	 0.0f
//arg2	float	spacing_w	=	 -1.0f
int cximgui_SameLine_2_nn(lua_State* L) {
	int __argi__ = 0;
	float pos_x = (float)lua_tonumber(L, __argi__++);
	float spacing_w = (float)lua_tonumber(L, __argi__++);
	ImGui::SameLine(pos_x, spacing_w);
	return 0;
};

//void  NewLine();
int cximgui_NewLine(lua_State* L) {
	ImGui::NewLine();
	return 0;
};

//void  Spacing();
int cximgui_Spacing(lua_State* L) {
	ImGui::Spacing();
	return 0;
};

//void  Dummy(const ImVec2& size);
//arg1	const ImVec2&	size	=	
int cximgui_Dummy_1_v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGui::Dummy(size);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 2;
};

//void  Indent(float indent_w);
//arg1	float	indent_w	=	 0.0f
int cximgui_Indent_1_n(lua_State* L) {
	int __argi__ = 0;
	float indent_w = (float)lua_tonumber(L, __argi__++);
	ImGui::Indent(indent_w);
	return 0;
};

//void  Unindent(float indent_w);
//arg1	float	indent_w	=	 0.0f
int cximgui_Unindent_1_n(lua_State* L) {
	int __argi__ = 0;
	float indent_w = (float)lua_tonumber(L, __argi__++);
	ImGui::Unindent(indent_w);
	return 0;
};

//void  BeginGroup();
int cximgui_BeginGroup(lua_State* L) {
	ImGui::BeginGroup();
	return 0;
};

//void  EndGroup();
int cximgui_EndGroup(lua_State* L) {
	ImGui::EndGroup();
	return 0;
};

//ImVec2  GetCursorPos();
int cximgui_GetCursorPos(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetCursorPos();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//float  GetCursorPosX();
int cximgui_GetCursorPosX(lua_State* L) {
	float __ret__ = ImGui::GetCursorPosX();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetCursorPosY();
int cximgui_GetCursorPosY(lua_State* L) {
	float __ret__ = ImGui::GetCursorPosY();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetCursorPos(const ImVec2& local_pos);
//arg1	const ImVec2&	local_pos	=	
int cximgui_SetCursorPos_1_v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 local_pos;
	local_pos.x = (float)lua_tonumber(L, __argi__++);
	local_pos.y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetCursorPos(local_pos);
	lua_pushnumber(L, local_pos.x);
	lua_pushnumber(L, local_pos.y);
	return 2;
};

//void  SetCursorPosX(float x);
//arg1	float	x	=	
int cximgui_SetCursorPosX_1_n(lua_State* L) {
	int __argi__ = 0;
	float x = (float)lua_tonumber(L, __argi__++);
	ImGui::SetCursorPosX(x);
	return 0;
};

//void  SetCursorPosY(float y);
//arg1	float	y	=	
int cximgui_SetCursorPosY_1_n(lua_State* L) {
	int __argi__ = 0;
	float y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetCursorPosY(y);
	return 0;
};

//ImVec2  GetCursorStartPos();
int cximgui_GetCursorStartPos(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetCursorStartPos();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetCursorScreenPos();
int cximgui_GetCursorScreenPos(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetCursorScreenPos();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//void  SetCursorScreenPos(const ImVec2& pos);
//arg1	const ImVec2&	pos	=	
int cximgui_SetCursorScreenPos_1_v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 pos;
	pos.x = (float)lua_tonumber(L, __argi__++);
	pos.y = (float)lua_tonumber(L, __argi__++);
	ImGui::SetCursorScreenPos(pos);
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
};

//void  AlignTextToFramePadding();
int cximgui_AlignTextToFramePadding(lua_State* L) {
	ImGui::AlignTextToFramePadding();
	return 0;
};

//float  GetTextLineHeight();
int cximgui_GetTextLineHeight(lua_State* L) {
	float __ret__ = ImGui::GetTextLineHeight();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetTextLineHeightWithSpacing();
int cximgui_GetTextLineHeightWithSpacing(lua_State* L) {
	float __ret__ = ImGui::GetTextLineHeightWithSpacing();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetFrameHeight();
int cximgui_GetFrameHeight(lua_State* L) {
	float __ret__ = ImGui::GetFrameHeight();
	lua_pushnumber(L, __ret__);
	return 1;
};

//float  GetFrameHeightWithSpacing();
int cximgui_GetFrameHeightWithSpacing(lua_State* L) {
	float __ret__ = ImGui::GetFrameHeightWithSpacing();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  PushID(const char* str_id);
//arg1	const char*	str_id	=	
int cximgui_PushID_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGui::PushID(str_id);
	return 0;
};

//void  PushID(const char* str_id_begin,const char* str_id_end);
//arg1	const char*	str_id_begin	=	
//arg2	const char*	str_id_end	=	
int cximgui_PushID_2_ss(lua_State* L) {
	int __argi__ = 0;
	const char* str_id_begin = lua_tostring(L, __argi__++);
	const char* str_id_end = lua_tostring(L, __argi__++);
	ImGui::PushID(str_id_begin, str_id_end);
	return 0;
};

//void  PushID(const void* ptr_id);
//arg1	const void*	ptr_id	=	
//UnSupported PushID
//void  PushID(int int_id);
//arg1	int	int_id	=	
int cximgui_PushID_1_i(lua_State* L) {
	int __argi__ = 0;
	int int_id = (int)lua_tointeger(L, __argi__++);
	ImGui::PushID(int_id);
	return 0;
};

//void  PopID();
int cximgui_PopID(lua_State* L) {
	ImGui::PopID();
	return 0;
};

//ImGuiID  GetID(const char* str_id);
//arg1	const char*	str_id	=	
int cximgui_GetID_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGuiID __ret__ = ImGui::GetID(str_id);
	lua_pushinteger(L, __ret__);
	return 1;
};

//ImGuiID  GetID(const char* str_id_begin,const char* str_id_end);
//arg1	const char*	str_id_begin	=	
//arg2	const char*	str_id_end	=	
int cximgui_GetID_2_ss(lua_State* L) {
	int __argi__ = 0;
	const char* str_id_begin = lua_tostring(L, __argi__++);
	const char* str_id_end = lua_tostring(L, __argi__++);
	ImGuiID __ret__ = ImGui::GetID(str_id_begin, str_id_end);
	lua_pushinteger(L, __ret__);
	return 1;
};

//ImGuiID  GetID(const void* ptr_id);
//arg1	const void*	ptr_id	=	
//UnSupported GetID
//void  TextUnformatted(const char* text,const char* text_end);
//arg1	const char*	text	=	
//arg2	const char*	text_end	=	 NULL
int cximgui_TextUnformatted_2_ss(lua_State* L) {
	int __argi__ = 0;
	const char* text = lua_tostring(L, __argi__++);
	const char* text_end = lua_tostring(L, __argi__++);
	ImGui::TextUnformatted(text, text_end);
	return 0;
};

//void  Text(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[Text] unsupported arg type!!!

//void  TextV(const char* fmt,va_list args);
//arg1	const char*	fmt	=	
//arg2	va_list	args	=	
//[TextV] unsupported arg type!!!

//void  TextColored(const ImVec4& col,const char* fmt,... );
//arg1	const ImVec4&	col	=	
//arg2	const char*	fmt	=	
//arg3	...		=	
//[TextColored] unsupported arg type!!!

//void  TextColoredV(const ImVec4& col,const char* fmt,va_list args);
//arg1	const ImVec4&	col	=	
//arg2	const char*	fmt	=	
//arg3	va_list	args	=	
//[TextColoredV] unsupported arg type!!!

//void  TextDisabled(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[TextDisabled] unsupported arg type!!!

//void  TextDisabledV(const char* fmt,va_list args);
//arg1	const char*	fmt	=	
//arg2	va_list	args	=	
//[TextDisabledV] unsupported arg type!!!

//void  TextWrapped(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[TextWrapped] unsupported arg type!!!

//void  TextWrappedV(const char* fmt,va_list args);
//arg1	const char*	fmt	=	
//arg2	va_list	args	=	
//[TextWrappedV] unsupported arg type!!!

//void  LabelText(const char* label,const char* fmt,... );
//arg1	const char*	label	=	
//arg2	const char*	fmt	=	
//arg3	...		=	
//[LabelText] unsupported arg type!!!

//void  LabelTextV(const char* label,const char* fmt,va_list args);
//arg1	const char*	label	=	
//arg2	const char*	fmt	=	
//arg3	va_list	args	=	
//[LabelTextV] unsupported arg type!!!

//void  BulletText(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[BulletText] unsupported arg type!!!

//void  BulletTextV(const char* fmt,va_list args);
//arg1	const char*	fmt	=	
//arg2	va_list	args	=	
//[BulletTextV] unsupported arg type!!!

//bool  Button(const char* label,const ImVec2& size);
//arg1	const char*	label	=	
//arg2	const ImVec2&	size	=	 ImVec2 @1
int cximgui_Button_2_sv2(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::Button(label, size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  SmallButton(const char* label);
//arg1	const char*	label	=	
int cximgui_SmallButton_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::SmallButton(label);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  InvisibleButton(const char* str_id,const ImVec2& size);
//arg1	const char*	str_id	=	
//arg2	const ImVec2&	size	=	
int cximgui_InvisibleButton_2_sv2(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::InvisibleButton(str_id, size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  ArrowButton(const char* str_id,ImGuiDir dir);
//arg1	const char*	str_id	=	
//arg2	ImGuiDir	dir	=	
int cximgui_ArrowButton_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGuiDir dir = (ImGuiDir)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::ArrowButton(str_id, dir);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  Image(ImTextureID user_texture_id,const ImVec2& size,const ImVec2& uv0,const ImVec2& uv1,const ImVec4& tint_col,const ImVec4& border_col);
//arg1	ImTextureID	user_texture_id	=	
//arg2	const ImVec2&	size	=	
//arg3	const ImVec2&	uv0	=	 ImVec2 @1
//arg4	const ImVec2&	uv1	=	 ImVec2 @2
//arg5	const ImVec4&	tint_col	=	 ImVec4 @3
//arg6	const ImVec4&	border_col	=	 ImVec4 @4
//[Image] unsupported arg type!!!

//bool  ImageButton(ImTextureID user_texture_id,const ImVec2& size,const ImVec2& uv0,const ImVec2& uv1,int frame_padding,const ImVec4& bg_col,const ImVec4& tint_col);
//arg1	ImTextureID	user_texture_id	=	
//arg2	const ImVec2&	size	=	
//arg3	const ImVec2&	uv0	=	 ImVec2 @1
//arg4	const ImVec2&	uv1	=	 ImVec2 @2
//arg5	int	frame_padding	=	 -1
//arg6	const ImVec4&	bg_col	=	 ImVec4 @3
//arg7	const ImVec4&	tint_col	=	 ImVec4 @4
//[ImageButton] unsupported arg type!!!

//bool  Checkbox(const char* label,bool* v);
//arg1	const char*	label	=	
//arg2	bool*	v	=	
int cximgui_Checkbox_2_sbp(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool v = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::Checkbox(label, &v);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, v);
	return 2;
};

//bool  CheckboxFlags(const char* label,unsigned int* flags,unsigned int flags_value);
//arg1	const char*	label	=	
//arg2	unsigned int*	flags	=	
//arg3	unsigned int	flags_value	=	
int cximgui_CheckboxFlags_3_sIpI(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	unsigned int flags = (unsigned int)lua_tointeger(L, __argi__++);
	unsigned int flags_value = (unsigned int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::CheckboxFlags(label, &flags, flags_value);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  RadioButton(const char* label,bool active);
//arg1	const char*	label	=	
//arg2	bool	active	=	
int cximgui_RadioButton_2_sb(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool active = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::RadioButton(label, active);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  RadioButton(const char* label,int* v,int v_button);
//arg1	const char*	label	=	
//arg2	int*	v	=	
//arg3	int	v_button	=	
int cximgui_RadioButton_3_sipi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	int v_button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::RadioButton(label, &v, v_button);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, v);
	return 2;
};

//void  ProgressBar(float fraction,const ImVec2& size_arg,const char* overlay);
//arg1	float	fraction	=	
//arg2	const ImVec2&	size_arg	=	 ImVec2 @1
//arg3	const char*	overlay	=	 NULL
int cximgui_ProgressBar_3_nv2s(lua_State* L) {
	int __argi__ = 0;
	float fraction = (float)lua_tonumber(L, __argi__++);
	ImVec2 size_arg;
	size_arg.x = (float)lua_tonumber(L, __argi__++);
	size_arg.y = (float)lua_tonumber(L, __argi__++);
	const char* overlay = lua_tostring(L, __argi__++);
	ImGui::ProgressBar(fraction, size_arg, overlay);
	lua_pushnumber(L, size_arg.x);
	lua_pushnumber(L, size_arg.y);
	return 2;
};

//void  Bullet();
int cximgui_Bullet(lua_State* L) {
	ImGui::Bullet();
	return 0;
};

//bool  BeginCombo(const char* label,const char* preview_value,ImGuiComboFlags flags);
//arg1	const char*	label	=	
//arg2	const char*	preview_value	=	
//arg3	ImGuiComboFlags	flags	=	 0
int cximgui_BeginCombo_3_ssi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	const char* preview_value = lua_tostring(L, __argi__++);
	ImGuiComboFlags flags = (ImGuiComboFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginCombo(label, preview_value, flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  EndCombo();
int cximgui_EndCombo(lua_State* L) {
	ImGui::EndCombo();
	return 0;
};

//bool  Combo(const char* label,int* current_item,const char* const[],int items_count,int popup_max_height_in_items);
//arg1	const char*	label	=	
//arg2	int*	current_item	=	
//arg3	const char*[0]	const	=	
//arg4	int	items_count	=	
//arg5	int	popup_max_height_in_items	=	 -1
//[Combo] unsupported arg type!!!

//bool  Combo(const char* label,int* current_item,const char* items_separated_by_zeros,int popup_max_height_in_items);
//arg1	const char*	label	=	
//arg2	int*	current_item	=	
//arg3	const char*	items_separated_by_zeros	=	
//arg4	int	popup_max_height_in_items	=	 -1
int cximgui_Combo_4_sipsi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int current_item = (int)lua_tointeger(L, __argi__++);
	const char* items_separated_by_zeros = lua_tostring(L, __argi__++);
	int popup_max_height_in_items = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::Combo(label, &current_item, items_separated_by_zeros, popup_max_height_in_items);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, current_item);
	return 2;
};

//bool  Combo(const char* label,int* current_item,bool @1@2,void* data,int items_count,int popup_max_height_in_items);
//arg1	const char*	label	=	
//arg2	int*	current_item	=	
//arg3	bool	@1@2	=	
//arg4	void*	data	=	
//arg5	int	items_count	=	
//arg6	int	popup_max_height_in_items	=	 -1
//UnSupported Combo
//bool  DragFloat(const char* label,float* v,float v_speed,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float*	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	float	v_min	=	 0.0f
//arg5	float	v_max	=	 0.0f
//arg6	const char*	format	=	 "%.3f"
//arg7	float	power	=	 1.0f
int cximgui_DragFloat_7_snpnnnsn(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	float v_speed = (float)lua_tonumber(L, __argi__++);
	float v_min = (float)lua_tonumber(L, __argi__++);
	float v_max = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	float power = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::DragFloat(label, &v, v_speed, v_min, v_max, format, power);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, v);
	return 2;
};

//bool  DragFloat2(const char* label,float v[2],float v_speed,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[2]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	float	v_min	=	 0.0f
//arg5	float	v_max	=	 0.0f
//arg6	const char*	format	=	 "%.3f"
//arg7	float	power	=	 1.0f
//[DragFloat2] unsupported arg type!!!

//bool  DragFloat3(const char* label,float v[3],float v_speed,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[3]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	float	v_min	=	 0.0f
//arg5	float	v_max	=	 0.0f
//arg6	const char*	format	=	 "%.3f"
//arg7	float	power	=	 1.0f
//[DragFloat3] unsupported arg type!!!

//bool  DragFloat4(const char* label,float v[4],float v_speed,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[4]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	float	v_min	=	 0.0f
//arg5	float	v_max	=	 0.0f
//arg6	const char*	format	=	 "%.3f"
//arg7	float	power	=	 1.0f
//[DragFloat4] unsupported arg type!!!

//bool  DragFloatRange2(const char* label,float* v_current_min,float* v_current_max,float v_speed,float v_min,float v_max,const char* format,const char* format_max,float power);
//arg1	const char*	label	=	
//arg2	float*	v_current_min	=	
//arg3	float*	v_current_max	=	
//arg4	float	v_speed	=	 1.0f
//arg5	float	v_min	=	 0.0f
//arg6	float	v_max	=	 0.0f
//arg7	const char*	format	=	 "%.3f"
//arg8	const char*	format_max	=	 NULL
//arg9	float	power	=	 1.0f
int cximgui_DragFloatRange2_9_snpnpnnnssn(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float v_current_min = (float)lua_tonumber(L, __argi__++);
	float v_current_max = (float)lua_tonumber(L, __argi__++);
	float v_speed = (float)lua_tonumber(L, __argi__++);
	float v_min = (float)lua_tonumber(L, __argi__++);
	float v_max = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	const char* format_max = lua_tostring(L, __argi__++);
	float power = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::DragFloatRange2(label, &v_current_min, &v_current_max, v_speed, v_min, v_max, format, format_max, power);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, v_current_min);
	lua_pushnumber(L, v_current_max);
	return 3;
};

//bool  DragInt(const char* label,int* v,float v_speed,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int*	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	int	v_min	=	 0
//arg5	int	v_max	=	 0
//arg6	const char*	format	=	 "%d"
int cximgui_DragInt_6_sipniis(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	float v_speed = (float)lua_tonumber(L, __argi__++);
	int v_min = (int)lua_tointeger(L, __argi__++);
	int v_max = (int)lua_tointeger(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::DragInt(label, &v, v_speed, v_min, v_max, format);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, v);
	return 2;
};

//bool  DragInt2(const char* label,int v[2],float v_speed,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[2]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	int	v_min	=	 0
//arg5	int	v_max	=	 0
//arg6	const char*	format	=	 "%d"
//[DragInt2] unsupported arg type!!!

//bool  DragInt3(const char* label,int v[3],float v_speed,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[3]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	int	v_min	=	 0
//arg5	int	v_max	=	 0
//arg6	const char*	format	=	 "%d"
//[DragInt3] unsupported arg type!!!

//bool  DragInt4(const char* label,int v[4],float v_speed,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[4]	v	=	
//arg3	float	v_speed	=	 1.0f
//arg4	int	v_min	=	 0
//arg5	int	v_max	=	 0
//arg6	const char*	format	=	 "%d"
//[DragInt4] unsupported arg type!!!

//bool  DragIntRange2(const char* label,int* v_current_min,int* v_current_max,float v_speed,int v_min,int v_max,const char* format,const char* format_max);
//arg1	const char*	label	=	
//arg2	int*	v_current_min	=	
//arg3	int*	v_current_max	=	
//arg4	float	v_speed	=	 1.0f
//arg5	int	v_min	=	 0
//arg6	int	v_max	=	 0
//arg7	const char*	format	=	 "%d"
//arg8	const char*	format_max	=	 NULL
int cximgui_DragIntRange2_8_sipipniiss(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int v_current_min = (int)lua_tointeger(L, __argi__++);
	int v_current_max = (int)lua_tointeger(L, __argi__++);
	float v_speed = (float)lua_tonumber(L, __argi__++);
	int v_min = (int)lua_tointeger(L, __argi__++);
	int v_max = (int)lua_tointeger(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	const char* format_max = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::DragIntRange2(label, &v_current_min, &v_current_max, v_speed, v_min, v_max, format, format_max);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, v_current_min);
	lua_pushinteger(L, v_current_max);
	return 3;
};

//bool  DragScalar(const char* label,ImGuiDataType data_type,void* v,float v_speed,const void* v_min,const void* v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	float	v_speed	=	
//arg5	const void*	v_min	=	 NULL
//arg6	const void*	v_max	=	 NULL
//arg7	const char*	format	=	 NULL
//arg8	float	power	=	 1.0f
//UnSupported DragScalar
//bool  DragScalarN(const char* label,ImGuiDataType data_type,void* v,int components,float v_speed,const void* v_min,const void* v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	int	components	=	
//arg5	float	v_speed	=	
//arg6	const void*	v_min	=	 NULL
//arg7	const void*	v_max	=	 NULL
//arg8	const char*	format	=	 NULL
//arg9	float	power	=	 1.0f
//UnSupported DragScalarN
//bool  SliderFloat(const char* label,float* v,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float*	v	=	
//arg3	float	v_min	=	
//arg4	float	v_max	=	
//arg5	const char*	format	=	 "%.3f"
//arg6	float	power	=	 1.0f
int cximgui_SliderFloat_6_snpnnsn(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	float v_min = (float)lua_tonumber(L, __argi__++);
	float v_max = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	float power = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::SliderFloat(label, &v, v_min, v_max, format, power);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, v);
	return 2;
};

//bool  SliderFloat2(const char* label,float v[2],float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[2]	v	=	
//arg3	float	v_min	=	
//arg4	float	v_max	=	
//arg5	const char*	format	=	 "%.3f"
//arg6	float	power	=	 1.0f
//[SliderFloat2] unsupported arg type!!!

//bool  SliderFloat3(const char* label,float v[3],float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[3]	v	=	
//arg3	float	v_min	=	
//arg4	float	v_max	=	
//arg5	const char*	format	=	 "%.3f"
//arg6	float	power	=	 1.0f
//[SliderFloat3] unsupported arg type!!!

//bool  SliderFloat4(const char* label,float v[4],float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	float[4]	v	=	
//arg3	float	v_min	=	
//arg4	float	v_max	=	
//arg5	const char*	format	=	 "%.3f"
//arg6	float	power	=	 1.0f
//[SliderFloat4] unsupported arg type!!!

//bool  SliderAngle(const char* label,float* v_rad,float v_degrees_min,float v_degrees_max,const char* format);
//arg1	const char*	label	=	
//arg2	float*	v_rad	=	
//arg3	float	v_degrees_min	=	 -360.0f
//arg4	float	v_degrees_max	=	 +360.0f
//arg5	const char*	format	=	 "%.0f deg"
int cximgui_SliderAngle_5_snpnns(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float v_rad = (float)lua_tonumber(L, __argi__++);
	float v_degrees_min = (float)lua_tonumber(L, __argi__++);
	float v_degrees_max = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::SliderAngle(label, &v_rad, v_degrees_min, v_degrees_max, format);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, v_rad);
	return 2;
};

//bool  SliderInt(const char* label,int* v,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int*	v	=	
//arg3	int	v_min	=	
//arg4	int	v_max	=	
//arg5	const char*	format	=	 "%d"
int cximgui_SliderInt_5_sipiis(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	int v_min = (int)lua_tointeger(L, __argi__++);
	int v_max = (int)lua_tointeger(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::SliderInt(label, &v, v_min, v_max, format);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, v);
	return 2;
};

//bool  SliderInt2(const char* label,int v[2],int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[2]	v	=	
//arg3	int	v_min	=	
//arg4	int	v_max	=	
//arg5	const char*	format	=	 "%d"
//[SliderInt2] unsupported arg type!!!

//bool  SliderInt3(const char* label,int v[3],int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[3]	v	=	
//arg3	int	v_min	=	
//arg4	int	v_max	=	
//arg5	const char*	format	=	 "%d"
//[SliderInt3] unsupported arg type!!!

//bool  SliderInt4(const char* label,int v[4],int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	int[4]	v	=	
//arg3	int	v_min	=	
//arg4	int	v_max	=	
//arg5	const char*	format	=	 "%d"
//[SliderInt4] unsupported arg type!!!

//bool  SliderScalar(const char* label,ImGuiDataType data_type,void* v,const void* v_min,const void* v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	const void*	v_min	=	
//arg5	const void*	v_max	=	
//arg6	const char*	format	=	 NULL
//arg7	float	power	=	 1.0f
//UnSupported SliderScalar
//bool  SliderScalarN(const char* label,ImGuiDataType data_type,void* v,int components,const void* v_min,const void* v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	int	components	=	
//arg5	const void*	v_min	=	
//arg6	const void*	v_max	=	
//arg7	const char*	format	=	 NULL
//arg8	float	power	=	 1.0f
//UnSupported SliderScalarN
//bool  VSliderFloat(const char* label,const ImVec2& size,float* v,float v_min,float v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	const ImVec2&	size	=	
//arg3	float*	v	=	
//arg4	float	v_min	=	
//arg5	float	v_max	=	
//arg6	const char*	format	=	 "%.3f"
//arg7	float	power	=	 1.0f
int cximgui_VSliderFloat_7_sv2npnnsn(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	float v_min = (float)lua_tonumber(L, __argi__++);
	float v_max = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	float power = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::VSliderFloat(label, size, &v, v_min, v_max, format, power);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	lua_pushnumber(L, v);
	return 4;
};

//bool  VSliderInt(const char* label,const ImVec2& size,int* v,int v_min,int v_max,const char* format);
//arg1	const char*	label	=	
//arg2	const ImVec2&	size	=	
//arg3	int*	v	=	
//arg4	int	v_min	=	
//arg5	int	v_max	=	
//arg6	const char*	format	=	 "%d"
int cximgui_VSliderInt_6_sv2ipiis(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	int v_min = (int)lua_tointeger(L, __argi__++);
	int v_max = (int)lua_tointeger(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::VSliderInt(label, size, &v, v_min, v_max, format);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	lua_pushinteger(L, v);
	return 4;
};

//bool  VSliderScalar(const char* label,const ImVec2& size,ImGuiDataType data_type,void* v,const void* v_min,const void* v_max,const char* format,float power);
//arg1	const char*	label	=	
//arg2	const ImVec2&	size	=	
//arg3	ImGuiDataType	data_type	=	
//arg4	void*	v	=	
//arg5	const void*	v_min	=	
//arg6	const void*	v_max	=	
//arg7	const char*	format	=	 NULL
//arg8	float	power	=	 1.0f
//UnSupported VSliderScalar
//bool  InputText(const char* label,char* buf,size_t buf_size,ImGuiInputTextFlags flags,ImGuiInputTextCallback callback,void* user_data);
//arg1	const char*	label	=	
//arg2	char*	buf	=	
//arg3	size_t	buf_size	=	
//arg4	ImGuiInputTextFlags	flags	=	 0
//arg5	ImGuiInputTextCallback	callback	=	 NULL
//arg6	void*	user_data	=	 NULL
//UnSupported InputText
//bool  InputTextMultiline(const char* label,char* buf,size_t buf_size,const ImVec2& size,ImGuiInputTextFlags flags,ImGuiInputTextCallback callback,void* user_data);
//arg1	const char*	label	=	
//arg2	char*	buf	=	
//arg3	size_t	buf_size	=	
//arg4	const ImVec2&	size	=	 ImVec2 @1
//arg5	ImGuiInputTextFlags	flags	=	 0
//arg6	ImGuiInputTextCallback	callback	=	 NULL
//arg7	void*	user_data	=	 NULL
//UnSupported InputTextMultiline
//bool  InputFloat(const char* label,float* v,float step,float step_fast,const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	float*	v	=	
//arg3	float	step	=	 0.0f
//arg4	float	step_fast	=	 0.0f
//arg5	const char*	format	=	 "%.3f"
//arg6	ImGuiInputTextFlags	extra_flags	=	 0
int cximgui_InputFloat_6_snpnnsi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	float step = (float)lua_tonumber(L, __argi__++);
	float step_fast = (float)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	ImGuiInputTextFlags extra_flags = (ImGuiInputTextFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::InputFloat(label, &v, step, step_fast, format, extra_flags);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, v);
	return 2;
};

//bool  InputFloat2(const char* label,float v[2],const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	float[2]	v	=	
//arg3	const char*	format	=	 "%.3f"
//arg4	ImGuiInputTextFlags	extra_flags	=	 0
//[InputFloat2] unsupported arg type!!!

//bool  InputFloat3(const char* label,float v[3],const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	float[3]	v	=	
//arg3	const char*	format	=	 "%.3f"
//arg4	ImGuiInputTextFlags	extra_flags	=	 0
//[InputFloat3] unsupported arg type!!!

//bool  InputFloat4(const char* label,float v[4],const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	float[4]	v	=	
//arg3	const char*	format	=	 "%.3f"
//arg4	ImGuiInputTextFlags	extra_flags	=	 0
//[InputFloat4] unsupported arg type!!!

//bool  InputInt(const char* label,int* v,int step,int step_fast,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	int*	v	=	
//arg3	int	step	=	 1
//arg4	int	step_fast	=	 100
//arg5	ImGuiInputTextFlags	extra_flags	=	 0
int cximgui_InputInt_5_sipiii(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	int step = (int)lua_tointeger(L, __argi__++);
	int step_fast = (int)lua_tointeger(L, __argi__++);
	ImGuiInputTextFlags extra_flags = (ImGuiInputTextFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::InputInt(label, &v, step, step_fast, extra_flags);
	lua_pushboolean(L, __ret__);
	lua_pushinteger(L, v);
	return 2;
};

//bool  InputInt2(const char* label,int v[2],ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	int[2]	v	=	
//arg3	ImGuiInputTextFlags	extra_flags	=	 0
//[InputInt2] unsupported arg type!!!

//bool  InputInt3(const char* label,int v[3],ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	int[3]	v	=	
//arg3	ImGuiInputTextFlags	extra_flags	=	 0
//[InputInt3] unsupported arg type!!!

//bool  InputInt4(const char* label,int v[4],ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	int[4]	v	=	
//arg3	ImGuiInputTextFlags	extra_flags	=	 0
//[InputInt4] unsupported arg type!!!

//bool  InputDouble(const char* label,double* v,double step,double step_fast,const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	double*	v	=	
//arg3	double	step	=	 0.0f
//arg4	double	step_fast	=	 0.0f
//arg5	const char*	format	=	 "%.6f"
//arg6	ImGuiInputTextFlags	extra_flags	=	 0
int cximgui_InputDouble_6_snpnnsi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	double v = (double)lua_tonumber(L, __argi__++);
	double step = (double)lua_tonumber(L, __argi__++);
	double step_fast = (double)lua_tonumber(L, __argi__++);
	const char* format = lua_tostring(L, __argi__++);
	ImGuiInputTextFlags extra_flags = (ImGuiInputTextFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::InputDouble(label, &v, step, step_fast, format, extra_flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  InputScalar(const char* label,ImGuiDataType data_type,void* v,const void* step,const void* step_fast,const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	const void*	step	=	 NULL
//arg5	const void*	step_fast	=	 NULL
//arg6	const char*	format	=	 NULL
//arg7	ImGuiInputTextFlags	extra_flags	=	 0
//UnSupported InputScalar
//bool  InputScalarN(const char* label,ImGuiDataType data_type,void* v,int components,const void* step,const void* step_fast,const char* format,ImGuiInputTextFlags extra_flags);
//arg1	const char*	label	=	
//arg2	ImGuiDataType	data_type	=	
//arg3	void*	v	=	
//arg4	int	components	=	
//arg5	const void*	step	=	 NULL
//arg6	const void*	step_fast	=	 NULL
//arg7	const char*	format	=	 NULL
//arg8	ImGuiInputTextFlags	extra_flags	=	 0
//UnSupported InputScalarN
//bool  ColorEdit3(const char* label,float col[3],ImGuiColorEditFlags flags);
//arg1	const char*	label	=	
//arg2	float[3]	col	=	
//arg3	ImGuiColorEditFlags	flags	=	 0
//[ColorEdit3] unsupported arg type!!!

//bool  ColorEdit4(const char* label,float col[4],ImGuiColorEditFlags flags);
//arg1	const char*	label	=	
//arg2	float[4]	col	=	
//arg3	ImGuiColorEditFlags	flags	=	 0
//[ColorEdit4] unsupported arg type!!!

//bool  ColorPicker3(const char* label,float col[3],ImGuiColorEditFlags flags);
//arg1	const char*	label	=	
//arg2	float[3]	col	=	
//arg3	ImGuiColorEditFlags	flags	=	 0
//[ColorPicker3] unsupported arg type!!!

//bool  ColorPicker4(const char* label,float col[4],ImGuiColorEditFlags flags,const float* ref_col);
//arg1	const char*	label	=	
//arg2	float[4]	col	=	
//arg3	ImGuiColorEditFlags	flags	=	 0
//arg4	const float*	ref_col	=	 NULL
//[ColorPicker4] unsupported arg type!!!

//bool  ColorButton(const char* desc_id,const ImVec4& col,ImGuiColorEditFlags flags,ImVec2 size);
//arg1	const char*	desc_id	=	
//arg2	const ImVec4&	col	=	
//arg3	ImGuiColorEditFlags	flags	=	 0
//arg4	ImVec2	size	=	 ImVec2 @1
int cximgui_ColorButton_4_sv4iv2(lua_State* L) {
	int __argi__ = 0;
	const char* desc_id = lua_tostring(L, __argi__++);
	ImVec4 col;
	col.x = (float)lua_tonumber(L, __argi__++);
	col.y = (float)lua_tonumber(L, __argi__++);
	col.z = (float)lua_tonumber(L, __argi__++);
	col.w = (float)lua_tonumber(L, __argi__++);
	ImGuiColorEditFlags flags = (ImGuiColorEditFlags)lua_tointeger(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::ColorButton(desc_id, col, flags, size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, col.x);
	lua_pushnumber(L, col.y);
	lua_pushnumber(L, col.z);
	lua_pushnumber(L, col.w);
	return 5;
};

//void  SetColorEditOptions(ImGuiColorEditFlags flags);
//arg1	ImGuiColorEditFlags	flags	=	
int cximgui_SetColorEditOptions_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiColorEditFlags flags = (ImGuiColorEditFlags)lua_tointeger(L, __argi__++);
	ImGui::SetColorEditOptions(flags);
	return 0;
};

//bool  TreeNode(const char* label);
//arg1	const char*	label	=	
int cximgui_TreeNode_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::TreeNode(label);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  TreeNode(const char* str_id,const char* fmt,... );
//arg1	const char*	str_id	=	
//arg2	const char*	fmt	=	
//arg3	...		=	
//[TreeNode] unsupported arg type!!!

//bool  TreeNode(const void* ptr_id,const char* fmt,... );
//arg1	const void*	ptr_id	=	
//arg2	const char*	fmt	=	
//arg3	...		=	
//UnSupported TreeNode
//bool  TreeNodeV(const char* str_id,const char* fmt,va_list args);
//arg1	const char*	str_id	=	
//arg2	const char*	fmt	=	
//arg3	va_list	args	=	
//[TreeNodeV] unsupported arg type!!!

//bool  TreeNodeV(const void* ptr_id,const char* fmt,va_list args);
//arg1	const void*	ptr_id	=	
//arg2	const char*	fmt	=	
//arg3	va_list	args	=	
//UnSupported TreeNodeV
//bool  TreeNodeEx(const char* label,ImGuiTreeNodeFlags flags);
//arg1	const char*	label	=	
//arg2	ImGuiTreeNodeFlags	flags	=	 0
int cximgui_TreeNodeEx_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImGuiTreeNodeFlags flags = (ImGuiTreeNodeFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::TreeNodeEx(label, flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  TreeNodeEx(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,... );
//arg1	const char*	str_id	=	
//arg2	ImGuiTreeNodeFlags	flags	=	
//arg3	const char*	fmt	=	
//arg4	...		=	
//[TreeNodeEx] unsupported arg type!!!

//bool  TreeNodeEx(const void* ptr_id,ImGuiTreeNodeFlags flags,const char* fmt,... );
//arg1	const void*	ptr_id	=	
//arg2	ImGuiTreeNodeFlags	flags	=	
//arg3	const char*	fmt	=	
//arg4	...		=	
//UnSupported TreeNodeEx
//bool  TreeNodeExV(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,va_list args);
//arg1	const char*	str_id	=	
//arg2	ImGuiTreeNodeFlags	flags	=	
//arg3	const char*	fmt	=	
//arg4	va_list	args	=	
//[TreeNodeExV] unsupported arg type!!!

//bool  TreeNodeExV(const void* ptr_id,ImGuiTreeNodeFlags flags,const char* fmt,va_list args);
//arg1	const void*	ptr_id	=	
//arg2	ImGuiTreeNodeFlags	flags	=	
//arg3	const char*	fmt	=	
//arg4	va_list	args	=	
//UnSupported TreeNodeExV
//void  TreePush(const char* str_id);
//arg1	const char*	str_id	=	
int cximgui_TreePush_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGui::TreePush(str_id);
	return 0;
};

//void  TreePush(const void* ptr_id);
//arg1	const void*	ptr_id	=	 NULL
//UnSupported TreePush
//void  TreePop();
int cximgui_TreePop(lua_State* L) {
	ImGui::TreePop();
	return 0;
};

//void  TreeAdvanceToLabelPos();
int cximgui_TreeAdvanceToLabelPos(lua_State* L) {
	ImGui::TreeAdvanceToLabelPos();
	return 0;
};

//float  GetTreeNodeToLabelSpacing();
int cximgui_GetTreeNodeToLabelSpacing(lua_State* L) {
	float __ret__ = ImGui::GetTreeNodeToLabelSpacing();
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetNextTreeNodeOpen(bool is_open,ImGuiCond cond);
//arg1	bool	is_open	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetNextTreeNodeOpen_2_bi(lua_State* L) {
	int __argi__ = 0;
	bool is_open = lua_toboolean(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetNextTreeNodeOpen(is_open, cond);
	return 0;
};

//bool  CollapsingHeader(const char* label,ImGuiTreeNodeFlags flags);
//arg1	const char*	label	=	
//arg2	ImGuiTreeNodeFlags	flags	=	 0
int cximgui_CollapsingHeader_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImGuiTreeNodeFlags flags = (ImGuiTreeNodeFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::CollapsingHeader(label, flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  CollapsingHeader(const char* label,bool* p_open,ImGuiTreeNodeFlags flags);
//arg1	const char*	label	=	
//arg2	bool*	p_open	=	
//arg3	ImGuiTreeNodeFlags	flags	=	 0
int cximgui_CollapsingHeader_3_sbpi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool p_open = lua_toboolean(L, __argi__++);
	ImGuiTreeNodeFlags flags = (ImGuiTreeNodeFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::CollapsingHeader(label, &p_open, flags);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_open);
	return 2;
};

//bool  Selectable(const char* label,bool selected,ImGuiSelectableFlags flags,const ImVec2& size);
//arg1	const char*	label	=	
//arg2	bool	selected	=	 false
//arg3	ImGuiSelectableFlags	flags	=	 0
//arg4	const ImVec2&	size	=	 ImVec2 @1
int cximgui_Selectable_4_sbiv2(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool selected = lua_toboolean(L, __argi__++);
	ImGuiSelectableFlags flags = (ImGuiSelectableFlags)lua_tointeger(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::Selectable(label, selected, flags, size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  Selectable(const char* label,bool* p_selected,ImGuiSelectableFlags flags,const ImVec2& size);
//arg1	const char*	label	=	
//arg2	bool*	p_selected	=	
//arg3	ImGuiSelectableFlags	flags	=	 0
//arg4	const ImVec2&	size	=	 ImVec2 @1
int cximgui_Selectable_4_sbpiv2(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool p_selected = lua_toboolean(L, __argi__++);
	ImGuiSelectableFlags flags = (ImGuiSelectableFlags)lua_tointeger(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::Selectable(label, &p_selected, flags, size);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_selected);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 4;
};

//bool  ListBox(const char* label,int* current_item,const char* const[],int items_count,int height_in_items);
//arg1	const char*	label	=	
//arg2	int*	current_item	=	
//arg3	const char*[0]	const	=	
//arg4	int	items_count	=	
//arg5	int	height_in_items	=	 -1
//[ListBox] unsupported arg type!!!

//bool  ListBox(const char* label,int* current_item,bool @1@2,void* data,int items_count,int height_in_items);
//arg1	const char*	label	=	
//arg2	int*	current_item	=	
//arg3	bool	@1@2	=	
//arg4	void*	data	=	
//arg5	int	items_count	=	
//arg6	int	height_in_items	=	 -1
//UnSupported ListBox
//bool  ListBoxHeader(const char* label,const ImVec2& size);
//arg1	const char*	label	=	
//arg2	const ImVec2&	size	=	 ImVec2 @1
int cximgui_ListBoxHeader_2_sv2(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::ListBoxHeader(label, size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  ListBoxHeader(const char* label,int items_count,int height_in_items);
//arg1	const char*	label	=	
//arg2	int	items_count	=	
//arg3	int	height_in_items	=	 -1
int cximgui_ListBoxHeader_3_sii(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	int items_count = (int)lua_tointeger(L, __argi__++);
	int height_in_items = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::ListBoxHeader(label, items_count, height_in_items);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  ListBoxFooter();
int cximgui_ListBoxFooter(lua_State* L) {
	ImGui::ListBoxFooter();
	return 0;
};

//void  PlotLines(const char* label,const float* values,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size,int stride);
//arg1	const char*	label	=	
//arg2	const float*	values	=	
//arg3	int	values_count	=	
//arg4	int	values_offset	=	 0
//arg5	const char*	overlay_text	=	 NULL
//arg6	float	scale_min	=	 FLT_MAX
//arg7	float	scale_max	=	 FLT_MAX
//arg8	ImVec2	graph_size	=	 ImVec2 @1
//arg9	int	stride	=	 sizeof @2
int cximgui_PlotLines_9_snpiisnnv2i(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float values = (float)lua_tonumber(L, __argi__++);
	int values_count = (int)lua_tointeger(L, __argi__++);
	int values_offset = (int)lua_tointeger(L, __argi__++);
	const char* overlay_text = lua_tostring(L, __argi__++);
	float scale_min = (float)lua_tonumber(L, __argi__++);
	float scale_max = (float)lua_tonumber(L, __argi__++);
	ImVec2 graph_size;
	graph_size.x = (float)lua_tonumber(L, __argi__++);
	graph_size.y = (float)lua_tonumber(L, __argi__++);
	int stride = (int)lua_tointeger(L, __argi__++);
	ImGui::PlotLines(label, &values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	return 0;
};

//void  PlotLines(const char* label,float @1@2,void* data,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size);
//arg1	const char*	label	=	
//arg2	float	@1@2	=	
//arg3	void*	data	=	
//arg4	int	values_count	=	
//arg5	int	values_offset	=	 0
//arg6	const char*	overlay_text	=	 NULL
//arg7	float	scale_min	=	 FLT_MAX
//arg8	float	scale_max	=	 FLT_MAX
//arg9	ImVec2	graph_size	=	 ImVec2 @3
//UnSupported PlotLines
//void  PlotHistogram(const char* label,const float* values,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size,int stride);
//arg1	const char*	label	=	
//arg2	const float*	values	=	
//arg3	int	values_count	=	
//arg4	int	values_offset	=	 0
//arg5	const char*	overlay_text	=	 NULL
//arg6	float	scale_min	=	 FLT_MAX
//arg7	float	scale_max	=	 FLT_MAX
//arg8	ImVec2	graph_size	=	 ImVec2 @1
//arg9	int	stride	=	 sizeof @2
int cximgui_PlotHistogram_9_snpiisnnv2i(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	float values = (float)lua_tonumber(L, __argi__++);
	int values_count = (int)lua_tointeger(L, __argi__++);
	int values_offset = (int)lua_tointeger(L, __argi__++);
	const char* overlay_text = lua_tostring(L, __argi__++);
	float scale_min = (float)lua_tonumber(L, __argi__++);
	float scale_max = (float)lua_tonumber(L, __argi__++);
	ImVec2 graph_size;
	graph_size.x = (float)lua_tonumber(L, __argi__++);
	graph_size.y = (float)lua_tonumber(L, __argi__++);
	int stride = (int)lua_tointeger(L, __argi__++);
	ImGui::PlotHistogram(label, &values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
	return 0;
};

//void  PlotHistogram(const char* label,float @1@2,void* data,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size);
//arg1	const char*	label	=	
//arg2	float	@1@2	=	
//arg3	void*	data	=	
//arg4	int	values_count	=	
//arg5	int	values_offset	=	 0
//arg6	const char*	overlay_text	=	 NULL
//arg7	float	scale_min	=	 FLT_MAX
//arg8	float	scale_max	=	 FLT_MAX
//arg9	ImVec2	graph_size	=	 ImVec2 @3
//UnSupported PlotHistogram
//void  Value(const char* prefix,bool b);
//arg1	const char*	prefix	=	
//arg2	bool	b	=	
int cximgui_Value_2_sb(lua_State* L) {
	int __argi__ = 0;
	const char* prefix = lua_tostring(L, __argi__++);
	bool b = lua_toboolean(L, __argi__++);
	ImGui::Value(prefix, b);
	return 0;
};

//void  Value(const char* prefix,int v);
//arg1	const char*	prefix	=	
//arg2	int	v	=	
int cximgui_Value_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* prefix = lua_tostring(L, __argi__++);
	int v = (int)lua_tointeger(L, __argi__++);
	ImGui::Value(prefix, v);
	return 0;
};

//void  Value(const char* prefix,unsigned int v);
//arg1	const char*	prefix	=	
//arg2	unsigned int	v	=	
int cximgui_Value_2_sI(lua_State* L) {
	int __argi__ = 0;
	const char* prefix = lua_tostring(L, __argi__++);
	unsigned int v = (unsigned int)lua_tointeger(L, __argi__++);
	ImGui::Value(prefix, v);
	return 0;
};

//void  Value(const char* prefix,float v,const char* float_format);
//arg1	const char*	prefix	=	
//arg2	float	v	=	
//arg3	const char*	float_format	=	 NULL
int cximgui_Value_3_sns(lua_State* L) {
	int __argi__ = 0;
	const char* prefix = lua_tostring(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	const char* float_format = lua_tostring(L, __argi__++);
	ImGui::Value(prefix, v, float_format);
	return 0;
};

//bool  BeginMainMenuBar();
int cximgui_BeginMainMenuBar(lua_State* L) {
	bool __ret__ = ImGui::BeginMainMenuBar();
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  EndMainMenuBar();
int cximgui_EndMainMenuBar(lua_State* L) {
	ImGui::EndMainMenuBar();
	return 0;
};

//bool  BeginMenuBar();
int cximgui_BeginMenuBar(lua_State* L) {
	bool __ret__ = ImGui::BeginMenuBar();
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  EndMenuBar();
int cximgui_EndMenuBar(lua_State* L) {
	ImGui::EndMenuBar();
	return 0;
};

//bool  BeginMenu(const char* label,bool enabled);
//arg1	const char*	label	=	
//arg2	bool	enabled	=	 true
int cximgui_BeginMenu_2_sb(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool enabled = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::BeginMenu(label, enabled);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  EndMenu();
int cximgui_EndMenu(lua_State* L) {
	ImGui::EndMenu();
	return 0;
};

//bool  MenuItem(const char* label,const char* shortcut,bool selected,bool enabled);
//arg1	const char*	label	=	
//arg2	const char*	shortcut	=	 NULL
//arg3	bool	selected	=	 false
//arg4	bool	enabled	=	 true
int cximgui_MenuItem_4_ssbb(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	const char* shortcut = lua_tostring(L, __argi__++);
	bool selected = lua_toboolean(L, __argi__++);
	bool enabled = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::MenuItem(label, shortcut, selected, enabled);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  MenuItem(const char* label,const char* shortcut,bool* p_selected,bool enabled);
//arg1	const char*	label	=	
//arg2	const char*	shortcut	=	
//arg3	bool*	p_selected	=	
//arg4	bool	enabled	=	 true
int cximgui_MenuItem_4_ssbpb(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	const char* shortcut = lua_tostring(L, __argi__++);
	bool p_selected = lua_toboolean(L, __argi__++);
	bool enabled = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::MenuItem(label, shortcut, &p_selected, enabled);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_selected);
	return 2;
};

//void  BeginTooltip();
int cximgui_BeginTooltip(lua_State* L) {
	ImGui::BeginTooltip();
	return 0;
};

//void  EndTooltip();
int cximgui_EndTooltip(lua_State* L) {
	ImGui::EndTooltip();
	return 0;
};

//void  SetTooltip(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[SetTooltip] unsupported arg type!!!

//void  SetTooltipV(const char* fmt,va_list args);
//arg1	const char*	fmt	=	
//arg2	va_list	args	=	
//[SetTooltipV] unsupported arg type!!!

//void  OpenPopup(const char* str_id);
//arg1	const char*	str_id	=	
int cximgui_OpenPopup_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGui::OpenPopup(str_id);
	return 0;
};

//bool  BeginPopup(const char* str_id,ImGuiWindowFlags flags);
//arg1	const char*	str_id	=	
//arg2	ImGuiWindowFlags	flags	=	 0
int cximgui_BeginPopup_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginPopup(str_id, flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  BeginPopupContextItem(const char* str_id,int mouse_button);
//arg1	const char*	str_id	=	 NULL
//arg2	int	mouse_button	=	 1
int cximgui_BeginPopupContextItem_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	int mouse_button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginPopupContextItem(str_id, mouse_button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  BeginPopupContextWindow(const char* str_id,int mouse_button,bool also_over_items);
//arg1	const char*	str_id	=	 NULL
//arg2	int	mouse_button	=	 1
//arg3	bool	also_over_items	=	 true
int cximgui_BeginPopupContextWindow_3_sib(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	int mouse_button = (int)lua_tointeger(L, __argi__++);
	bool also_over_items = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::BeginPopupContextWindow(str_id, mouse_button, also_over_items);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  BeginPopupContextVoid(const char* str_id,int mouse_button);
//arg1	const char*	str_id	=	 NULL
//arg2	int	mouse_button	=	 1
int cximgui_BeginPopupContextVoid_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	int mouse_button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginPopupContextVoid(str_id, mouse_button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  BeginPopupModal(const char* name,bool* p_open,ImGuiWindowFlags flags);
//arg1	const char*	name	=	
//arg2	bool*	p_open	=	 NULL
//arg3	ImGuiWindowFlags	flags	=	 0
int cximgui_BeginPopupModal_3_sbpi(lua_State* L) {
	int __argi__ = 0;
	const char* name = lua_tostring(L, __argi__++);
	bool p_open = lua_toboolean(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginPopupModal(name, &p_open, flags);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_open);
	return 2;
};

//void  EndPopup();
int cximgui_EndPopup(lua_State* L) {
	ImGui::EndPopup();
	return 0;
};

//bool  OpenPopupOnItemClick(const char* str_id,int mouse_button);
//arg1	const char*	str_id	=	 NULL
//arg2	int	mouse_button	=	 1
int cximgui_OpenPopupOnItemClick_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	int mouse_button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::OpenPopupOnItemClick(str_id, mouse_button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsPopupOpen(const char* str_id);
//arg1	const char*	str_id	=	
int cximgui_IsPopupOpen_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	bool __ret__ = ImGui::IsPopupOpen(str_id);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  CloseCurrentPopup();
int cximgui_CloseCurrentPopup(lua_State* L) {
	ImGui::CloseCurrentPopup();
	return 0;
};

//void  Columns(int count,const char* id,bool border);
//arg1	int	count	=	 1
//arg2	const char*	id	=	 NULL
//arg3	bool	border	=	 true
int cximgui_Columns_3_isb(lua_State* L) {
	int __argi__ = 0;
	int count = (int)lua_tointeger(L, __argi__++);
	const char* id = lua_tostring(L, __argi__++);
	bool border = lua_toboolean(L, __argi__++);
	ImGui::Columns(count, id, border);
	return 0;
};

//void  NextColumn();
int cximgui_NextColumn(lua_State* L) {
	ImGui::NextColumn();
	return 0;
};

//int  GetColumnIndex();
int cximgui_GetColumnIndex(lua_State* L) {
	int __ret__ = ImGui::GetColumnIndex();
	lua_pushinteger(L, __ret__);
	return 1;
};

//float  GetColumnWidth(int column_index);
//arg1	int	column_index	=	 -1
int cximgui_GetColumnWidth_1_i(lua_State* L) {
	int __argi__ = 0;
	int column_index = (int)lua_tointeger(L, __argi__++);
	float __ret__ = ImGui::GetColumnWidth(column_index);
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetColumnWidth(int column_index,float width);
//arg1	int	column_index	=	
//arg2	float	width	=	
int cximgui_SetColumnWidth_2_in(lua_State* L) {
	int __argi__ = 0;
	int column_index = (int)lua_tointeger(L, __argi__++);
	float width = (float)lua_tonumber(L, __argi__++);
	ImGui::SetColumnWidth(column_index, width);
	return 0;
};

//float  GetColumnOffset(int column_index);
//arg1	int	column_index	=	 -1
int cximgui_GetColumnOffset_1_i(lua_State* L) {
	int __argi__ = 0;
	int column_index = (int)lua_tointeger(L, __argi__++);
	float __ret__ = ImGui::GetColumnOffset(column_index);
	lua_pushnumber(L, __ret__);
	return 1;
};

//void  SetColumnOffset(int column_index,float offset_x);
//arg1	int	column_index	=	
//arg2	float	offset_x	=	
int cximgui_SetColumnOffset_2_in(lua_State* L) {
	int __argi__ = 0;
	int column_index = (int)lua_tointeger(L, __argi__++);
	float offset_x = (float)lua_tonumber(L, __argi__++);
	ImGui::SetColumnOffset(column_index, offset_x);
	return 0;
};

//int  GetColumnsCount();
int cximgui_GetColumnsCount(lua_State* L) {
	int __ret__ = ImGui::GetColumnsCount();
	lua_pushinteger(L, __ret__);
	return 1;
};

//bool  BeginTabBar(const char* str_id,ImGuiTabBarFlags flags);
//arg1	const char*	str_id	=	
//arg2	ImGuiTabBarFlags	flags	=	 0
int cximgui_BeginTabBar_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* str_id = lua_tostring(L, __argi__++);
	ImGuiTabBarFlags flags = (ImGuiTabBarFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginTabBar(str_id, flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  EndTabBar();
int cximgui_EndTabBar(lua_State* L) {
	ImGui::EndTabBar();
	return 0;
};

//bool  BeginTabItem(const char* label,bool* p_open,ImGuiTabItemFlags flags);
//arg1	const char*	label	=	
//arg2	bool*	p_open	=	 NULL
//arg3	ImGuiTabItemFlags	flags	=	 0
int cximgui_BeginTabItem_3_sbpi(lua_State* L) {
	int __argi__ = 0;
	const char* label = lua_tostring(L, __argi__++);
	bool p_open = lua_toboolean(L, __argi__++);
	ImGuiTabItemFlags flags = (ImGuiTabItemFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginTabItem(label, &p_open, flags);
	lua_pushboolean(L, __ret__);
	lua_pushboolean(L, p_open);
	return 2;
};

//void  EndTabItem();
int cximgui_EndTabItem(lua_State* L) {
	ImGui::EndTabItem();
	return 0;
};

//void  SetTabItemClosed(const char* tab_or_docked_window_label);
//arg1	const char*	tab_or_docked_window_label	=	
int cximgui_SetTabItemClosed_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* tab_or_docked_window_label = lua_tostring(L, __argi__++);
	ImGui::SetTabItemClosed(tab_or_docked_window_label);
	return 0;
};

//void  DockSpace(ImGuiID id,const ImVec2& size,ImGuiDockNodeFlags flags,const ImGuiDockFamily* dock_family);
//arg1	ImGuiID	id	=	
//arg2	const ImVec2&	size	=	 ImVec2 @1
//arg3	ImGuiDockNodeFlags	flags	=	 0
//arg4	const ImGuiDockFamily*	dock_family	=	 NULL
//[DockSpace] unsupported arg type!!!

//ImGuiID  DockSpaceOverViewport(ImGuiViewport* viewport,ImGuiDockNodeFlags dockspace_flags,const ImGuiDockFamily* dock_family);
//arg1	ImGuiViewport*	viewport	=	 NULL
//arg2	ImGuiDockNodeFlags	dockspace_flags	=	 0
//arg3	const ImGuiDockFamily*	dock_family	=	 NULL
//[DockSpaceOverViewport] unsupported arg type!!!

//void  SetNextWindowDockId(ImGuiID dock_id,ImGuiCond cond);
//arg1	ImGuiID	dock_id	=	
//arg2	ImGuiCond	cond	=	 0
int cximgui_SetNextWindowDockId_2_ii(lua_State* L) {
	int __argi__ = 0;
	ImGuiID dock_id = (ImGuiID)lua_tointeger(L, __argi__++);
	ImGuiCond cond = (ImGuiCond)lua_tointeger(L, __argi__++);
	ImGui::SetNextWindowDockId(dock_id, cond);
	return 0;
};

//void  SetNextWindowDockFamily(const ImGuiDockFamily* dock_family);
//arg1	const ImGuiDockFamily*	dock_family	=	
//[SetNextWindowDockFamily] unsupported arg type!!!

//ImGuiID  GetWindowDockId();
int cximgui_GetWindowDockId(lua_State* L) {
	ImGuiID __ret__ = ImGui::GetWindowDockId();
	lua_pushinteger(L, __ret__);
	return 1;
};

//bool  IsWindowDocked();
int cximgui_IsWindowDocked(lua_State* L) {
	bool __ret__ = ImGui::IsWindowDocked();
	lua_pushboolean(L, __ret__);
	return 1;
};

//void  LogToTTY(int max_depth);
//arg1	int	max_depth	=	 -1
int cximgui_LogToTTY_1_i(lua_State* L) {
	int __argi__ = 0;
	int max_depth = (int)lua_tointeger(L, __argi__++);
	ImGui::LogToTTY(max_depth);
	return 0;
};

//void  LogToFile(int max_depth,const char* filename);
//arg1	int	max_depth	=	 -1
//arg2	const char*	filename	=	 NULL
int cximgui_LogToFile_2_is(lua_State* L) {
	int __argi__ = 0;
	int max_depth = (int)lua_tointeger(L, __argi__++);
	const char* filename = lua_tostring(L, __argi__++);
	ImGui::LogToFile(max_depth, filename);
	return 0;
};

//void  LogToClipboard(int max_depth);
//arg1	int	max_depth	=	 -1
int cximgui_LogToClipboard_1_i(lua_State* L) {
	int __argi__ = 0;
	int max_depth = (int)lua_tointeger(L, __argi__++);
	ImGui::LogToClipboard(max_depth);
	return 0;
};

//void  LogFinish();
int cximgui_LogFinish(lua_State* L) {
	ImGui::LogFinish();
	return 0;
};

//void  LogButtons();
int cximgui_LogButtons(lua_State* L) {
	ImGui::LogButtons();
	return 0;
};

//void  LogText(const char* fmt,... );
//arg1	const char*	fmt	=	
//arg2	...		=	
//[LogText] unsupported arg type!!!

//bool  BeginDragDropSource(ImGuiDragDropFlags flags);
//arg1	ImGuiDragDropFlags	flags	=	 0
int cximgui_BeginDragDropSource_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiDragDropFlags flags = (ImGuiDragDropFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginDragDropSource(flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  SetDragDropPayload(const char* type,const void* data,size_t size,ImGuiCond cond);
//arg1	const char*	type	=	
//arg2	const void*	data	=	
//arg3	size_t	size	=	
//arg4	ImGuiCond	cond	=	 0
//UnSupported SetDragDropPayload
//void  EndDragDropSource();
int cximgui_EndDragDropSource(lua_State* L) {
	ImGui::EndDragDropSource();
	return 0;
};

//bool  BeginDragDropTarget();
int cximgui_BeginDragDropTarget(lua_State* L) {
	bool __ret__ = ImGui::BeginDragDropTarget();
	lua_pushboolean(L, __ret__);
	return 1;
};

//const ImGuiPayload*  AcceptDragDropPayload(const char* type,ImGuiDragDropFlags flags);
//arg1	const char*	type	=	
//arg2	ImGuiDragDropFlags	flags	=	 0
int cximgui_AcceptDragDropPayload_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* type = lua_tostring(L, __argi__++);
	ImGuiDragDropFlags flags = (ImGuiDragDropFlags)lua_tointeger(L, __argi__++);
	const ImGuiPayload* __ret__ = ImGui::AcceptDragDropPayload(type, flags);
	return 0;
};

//void  EndDragDropTarget();
int cximgui_EndDragDropTarget(lua_State* L) {
	ImGui::EndDragDropTarget();
	return 0;
};

//const ImGuiPayload*  GetDragDropPayload();
int cximgui_GetDragDropPayload(lua_State* L) {
	const ImGuiPayload* __ret__ = ImGui::GetDragDropPayload();
	return 0;
};

//void  PushClipRect(const ImVec2& clip_rect_min,const ImVec2& clip_rect_max,bool intersect_with_current_clip_rect);
//arg1	const ImVec2&	clip_rect_min	=	
//arg2	const ImVec2&	clip_rect_max	=	
//arg3	bool	intersect_with_current_clip_rect	=	
int cximgui_PushClipRect_3_v2v2b(lua_State* L) {
	int __argi__ = 0;
	ImVec2 clip_rect_min;
	clip_rect_min.x = (float)lua_tonumber(L, __argi__++);
	clip_rect_min.y = (float)lua_tonumber(L, __argi__++);
	ImVec2 clip_rect_max;
	clip_rect_max.x = (float)lua_tonumber(L, __argi__++);
	clip_rect_max.y = (float)lua_tonumber(L, __argi__++);
	bool intersect_with_current_clip_rect = lua_toboolean(L, __argi__++);
	ImGui::PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
	lua_pushnumber(L, clip_rect_min.x);
	lua_pushnumber(L, clip_rect_min.y);
	lua_pushnumber(L, clip_rect_max.x);
	lua_pushnumber(L, clip_rect_max.y);
	return 4;
};

//void  PopClipRect();
int cximgui_PopClipRect(lua_State* L) {
	ImGui::PopClipRect();
	return 0;
};

//void  SetItemDefaultFocus();
int cximgui_SetItemDefaultFocus(lua_State* L) {
	ImGui::SetItemDefaultFocus();
	return 0;
};

//void  SetKeyboardFocusHere(int offset);
//arg1	int	offset	=	 0
int cximgui_SetKeyboardFocusHere_1_i(lua_State* L) {
	int __argi__ = 0;
	int offset = (int)lua_tointeger(L, __argi__++);
	ImGui::SetKeyboardFocusHere(offset);
	return 0;
};

//bool  IsItemHovered(ImGuiHoveredFlags flags);
//arg1	ImGuiHoveredFlags	flags	=	 0
int cximgui_IsItemHovered_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiHoveredFlags flags = (ImGuiHoveredFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsItemHovered(flags);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemActive();
int cximgui_IsItemActive(lua_State* L) {
	bool __ret__ = ImGui::IsItemActive();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemFocused();
int cximgui_IsItemFocused(lua_State* L) {
	bool __ret__ = ImGui::IsItemFocused();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemClicked(int mouse_button);
//arg1	int	mouse_button	=	 0
int cximgui_IsItemClicked_1_i(lua_State* L) {
	int __argi__ = 0;
	int mouse_button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsItemClicked(mouse_button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemVisible();
int cximgui_IsItemVisible(lua_State* L) {
	bool __ret__ = ImGui::IsItemVisible();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemEdited();
int cximgui_IsItemEdited(lua_State* L) {
	bool __ret__ = ImGui::IsItemEdited();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemDeactivated();
int cximgui_IsItemDeactivated(lua_State* L) {
	bool __ret__ = ImGui::IsItemDeactivated();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsItemDeactivatedAfterEdit();
int cximgui_IsItemDeactivatedAfterEdit(lua_State* L) {
	bool __ret__ = ImGui::IsItemDeactivatedAfterEdit();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsAnyItemHovered();
int cximgui_IsAnyItemHovered(lua_State* L) {
	bool __ret__ = ImGui::IsAnyItemHovered();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsAnyItemActive();
int cximgui_IsAnyItemActive(lua_State* L) {
	bool __ret__ = ImGui::IsAnyItemActive();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsAnyItemFocused();
int cximgui_IsAnyItemFocused(lua_State* L) {
	bool __ret__ = ImGui::IsAnyItemFocused();
	lua_pushboolean(L, __ret__);
	return 1;
};

//ImVec2  GetItemRectMin();
int cximgui_GetItemRectMin(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetItemRectMin();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetItemRectMax();
int cximgui_GetItemRectMax(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetItemRectMax();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetItemRectSize();
int cximgui_GetItemRectSize(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetItemRectSize();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//void  SetItemAllowOverlap();
int cximgui_SetItemAllowOverlap(lua_State* L) {
	ImGui::SetItemAllowOverlap();
	return 0;
};

//bool  IsRectVisible(const ImVec2& size);
//arg1	const ImVec2&	size	=	
int cximgui_IsRectVisible_1_v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::IsRectVisible(size);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//bool  IsRectVisible(const ImVec2& rect_min,const ImVec2& rect_max);
//arg1	const ImVec2&	rect_min	=	
//arg2	const ImVec2&	rect_max	=	
int cximgui_IsRectVisible_2_v2v2(lua_State* L) {
	int __argi__ = 0;
	ImVec2 rect_min;
	rect_min.x = (float)lua_tonumber(L, __argi__++);
	rect_min.y = (float)lua_tonumber(L, __argi__++);
	ImVec2 rect_max;
	rect_max.x = (float)lua_tonumber(L, __argi__++);
	rect_max.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::IsRectVisible(rect_min, rect_max);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, rect_min.x);
	lua_pushnumber(L, rect_min.y);
	lua_pushnumber(L, rect_max.x);
	lua_pushnumber(L, rect_max.y);
	return 5;
};

//double  GetTime();
int cximgui_GetTime(lua_State* L) {
	double __ret__ = ImGui::GetTime();
	lua_pushnumber(L, __ret__);
	return 1;
};

//int  GetFrameCount();
int cximgui_GetFrameCount(lua_State* L) {
	int __ret__ = ImGui::GetFrameCount();
	lua_pushinteger(L, __ret__);
	return 1;
};

//ImDrawList*  GetOverlayDrawList();
int cximgui_GetOverlayDrawList(lua_State* L) {
	ImDrawList* __ret__ = ImGui::GetOverlayDrawList();
	return 0;
};

//ImDrawList*  GetOverlayDrawList(ImGuiViewport* viewport);
//arg1	ImGuiViewport*	viewport	=	
//[GetOverlayDrawList] unsupported arg type!!!

//ImDrawListSharedData*  GetDrawListSharedData();
int cximgui_GetDrawListSharedData(lua_State* L) {
	ImDrawListSharedData* __ret__ = ImGui::GetDrawListSharedData();
	return 0;
};

//const char*  GetStyleColorName(ImGuiCol idx);
//arg1	ImGuiCol	idx	=	
int cximgui_GetStyleColorName_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiCol idx = (ImGuiCol)lua_tointeger(L, __argi__++);
	const char* __ret__ = ImGui::GetStyleColorName(idx);
	lua_pushstring(L, __ret__);
	return 1;
};

//void  SetStateStorage(ImGuiStorage* storage);
//arg1	ImGuiStorage*	storage	=	
//[SetStateStorage] unsupported arg type!!!

//ImGuiStorage*  GetStateStorage();
int cximgui_GetStateStorage(lua_State* L) {
	ImGuiStorage* __ret__ = ImGui::GetStateStorage();
	return 0;
};

//ImVec2  CalcTextSize(const char* text,const char* text_end,bool hide_text_after_double_hash,float wrap_width);
//arg1	const char*	text	=	
//arg2	const char*	text_end	=	 NULL
//arg3	bool	hide_text_after_double_hash	=	 false
//arg4	float	wrap_width	=	 -1.0f
int cximgui_CalcTextSize_4_ssbn(lua_State* L) {
	int __argi__ = 0;
	const char* text = lua_tostring(L, __argi__++);
	const char* text_end = lua_tostring(L, __argi__++);
	bool hide_text_after_double_hash = lua_toboolean(L, __argi__++);
	float wrap_width = (float)lua_tonumber(L, __argi__++);
	ImVec2 __ret__ = ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//void  CalcListClipping(int items_count,float items_height,int* out_items_display_start,int* out_items_display_end);
//arg1	int	items_count	=	
//arg2	float	items_height	=	
//arg3	int*	out_items_display_start	=	
//arg4	int*	out_items_display_end	=	
int cximgui_CalcListClipping_4_inipip(lua_State* L) {
	int __argi__ = 0;
	int items_count = (int)lua_tointeger(L, __argi__++);
	float items_height = (float)lua_tonumber(L, __argi__++);
	int out_items_display_start = (int)lua_tointeger(L, __argi__++);
	int out_items_display_end = (int)lua_tointeger(L, __argi__++);
	ImGui::CalcListClipping(items_count, items_height, &out_items_display_start, &out_items_display_end);
	lua_pushinteger(L, out_items_display_start);
	lua_pushinteger(L, out_items_display_end);
	return 2;
};

//bool  BeginChildFrame(ImGuiID id,const ImVec2& size,ImGuiWindowFlags flags);
//arg1	ImGuiID	id	=	
//arg2	const ImVec2&	size	=	
//arg3	ImGuiWindowFlags	flags	=	 0
int cximgui_BeginChildFrame_3_iv2i(lua_State* L) {
	int __argi__ = 0;
	ImGuiID id = (ImGuiID)lua_tointeger(L, __argi__++);
	ImVec2 size;
	size.x = (float)lua_tonumber(L, __argi__++);
	size.y = (float)lua_tonumber(L, __argi__++);
	ImGuiWindowFlags flags = (ImGuiWindowFlags)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::BeginChildFrame(id, size, flags);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, size.x);
	lua_pushnumber(L, size.y);
	return 3;
};

//void  EndChildFrame();
int cximgui_EndChildFrame(lua_State* L) {
	ImGui::EndChildFrame();
	return 0;
};

//ImVec4  ColorConvertU32ToFloat4(ImU32 in);
//arg1	ImU32	in	=	
int cximgui_ColorConvertU32ToFloat4_1_i(lua_State* L) {
	int __argi__ = 0;
	ImU32 in = (ImU32)lua_tointeger(L, __argi__++);
	ImVec4 __ret__ = ImGui::ColorConvertU32ToFloat4(in);
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	lua_pushnumber(L, __ret__.z);
	lua_pushnumber(L, __ret__.w);
	return 4;
};

//ImU32  ColorConvertFloat4ToU32(const ImVec4& in);
//arg1	const ImVec4&	in	=	
int cximgui_ColorConvertFloat4ToU32_1_v4(lua_State* L) {
	int __argi__ = 0;
	ImVec4 in;
	in.x = (float)lua_tonumber(L, __argi__++);
	in.y = (float)lua_tonumber(L, __argi__++);
	in.z = (float)lua_tonumber(L, __argi__++);
	in.w = (float)lua_tonumber(L, __argi__++);
	ImU32 __ret__ = ImGui::ColorConvertFloat4ToU32(in);
	lua_pushinteger(L, __ret__);
	lua_pushnumber(L, in.x);
	lua_pushnumber(L, in.y);
	lua_pushnumber(L, in.z);
	lua_pushnumber(L, in.w);
	return 5;
};

//void  ColorConvertRGBtoHSV(float r,float g,float b,float& out_h,float& out_s,float& out_v);
//arg1	float	r	=	
//arg2	float	g	=	
//arg3	float	b	=	
//arg4	float&	out_h	=	
//arg5	float&	out_s	=	
//arg6	float&	out_v	=	
int cximgui_ColorConvertRGBtoHSV_6_nnnnnn(lua_State* L) {
	int __argi__ = 0;
	float r = (float)lua_tonumber(L, __argi__++);
	float g = (float)lua_tonumber(L, __argi__++);
	float b = (float)lua_tonumber(L, __argi__++);
	float out_h = (float)lua_tonumber(L, __argi__++);
	float out_s = (float)lua_tonumber(L, __argi__++);
	float out_v = (float)lua_tonumber(L, __argi__++);
	ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
	lua_pushnumber(L, out_h);
	lua_pushnumber(L, out_s);
	lua_pushnumber(L, out_v);
	return 3;
};

//void  ColorConvertHSVtoRGB(float h,float s,float v,float& out_r,float& out_g,float& out_b);
//arg1	float	h	=	
//arg2	float	s	=	
//arg3	float	v	=	
//arg4	float&	out_r	=	
//arg5	float&	out_g	=	
//arg6	float&	out_b	=	
int cximgui_ColorConvertHSVtoRGB_6_nnnnnn(lua_State* L) {
	int __argi__ = 0;
	float h = (float)lua_tonumber(L, __argi__++);
	float s = (float)lua_tonumber(L, __argi__++);
	float v = (float)lua_tonumber(L, __argi__++);
	float out_r = (float)lua_tonumber(L, __argi__++);
	float out_g = (float)lua_tonumber(L, __argi__++);
	float out_b = (float)lua_tonumber(L, __argi__++);
	ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
	lua_pushnumber(L, out_r);
	lua_pushnumber(L, out_g);
	lua_pushnumber(L, out_b);
	return 3;
};

//int  GetKeyIndex(ImGuiKey imgui_key);
//arg1	ImGuiKey	imgui_key	=	
int cximgui_GetKeyIndex_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiKey imgui_key = (ImGuiKey)lua_tointeger(L, __argi__++);
	int __ret__ = ImGui::GetKeyIndex(imgui_key);
	lua_pushinteger(L, __ret__);
	return 1;
};

//bool  IsKeyDown(int user_key_index);
//arg1	int	user_key_index	=	
int cximgui_IsKeyDown_1_i(lua_State* L) {
	int __argi__ = 0;
	int user_key_index = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsKeyDown(user_key_index);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsKeyPressed(int user_key_index,bool repeat);
//arg1	int	user_key_index	=	
//arg2	bool	repeat	=	 true
int cximgui_IsKeyPressed_2_ib(lua_State* L) {
	int __argi__ = 0;
	int user_key_index = (int)lua_tointeger(L, __argi__++);
	bool repeat = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::IsKeyPressed(user_key_index, repeat);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsKeyReleased(int user_key_index);
//arg1	int	user_key_index	=	
int cximgui_IsKeyReleased_1_i(lua_State* L) {
	int __argi__ = 0;
	int user_key_index = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsKeyReleased(user_key_index);
	lua_pushboolean(L, __ret__);
	return 1;
};

//int  GetKeyPressedAmount(int key_index,float repeat_delay,float rate);
//arg1	int	key_index	=	
//arg2	float	repeat_delay	=	
//arg3	float	rate	=	
int cximgui_GetKeyPressedAmount_3_inn(lua_State* L) {
	int __argi__ = 0;
	int key_index = (int)lua_tointeger(L, __argi__++);
	float repeat_delay = (float)lua_tonumber(L, __argi__++);
	float rate = (float)lua_tonumber(L, __argi__++);
	int __ret__ = ImGui::GetKeyPressedAmount(key_index, repeat_delay, rate);
	lua_pushinteger(L, __ret__);
	return 1;
};

//bool  IsMouseDown(int button);
//arg1	int	button	=	
int cximgui_IsMouseDown_1_i(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsMouseDown(button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsAnyMouseDown();
int cximgui_IsAnyMouseDown(lua_State* L) {
	bool __ret__ = ImGui::IsAnyMouseDown();
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsMouseClicked(int button,bool repeat);
//arg1	int	button	=	
//arg2	bool	repeat	=	 false
int cximgui_IsMouseClicked_2_ib(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	bool repeat = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::IsMouseClicked(button, repeat);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsMouseDoubleClicked(int button);
//arg1	int	button	=	
int cximgui_IsMouseDoubleClicked_1_i(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsMouseDoubleClicked(button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsMouseReleased(int button);
//arg1	int	button	=	
int cximgui_IsMouseReleased_1_i(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	bool __ret__ = ImGui::IsMouseReleased(button);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsMouseDragging(int button,float lock_threshold);
//arg1	int	button	=	 0
//arg2	float	lock_threshold	=	 -1.0f
int cximgui_IsMouseDragging_2_in(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	float lock_threshold = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::IsMouseDragging(button, lock_threshold);
	lua_pushboolean(L, __ret__);
	return 1;
};

//bool  IsMouseHoveringRect(const ImVec2& r_min,const ImVec2& r_max,bool clip);
//arg1	const ImVec2&	r_min	=	
//arg2	const ImVec2&	r_max	=	
//arg3	bool	clip	=	 true
int cximgui_IsMouseHoveringRect_3_v2v2b(lua_State* L) {
	int __argi__ = 0;
	ImVec2 r_min;
	r_min.x = (float)lua_tonumber(L, __argi__++);
	r_min.y = (float)lua_tonumber(L, __argi__++);
	ImVec2 r_max;
	r_max.x = (float)lua_tonumber(L, __argi__++);
	r_max.y = (float)lua_tonumber(L, __argi__++);
	bool clip = lua_toboolean(L, __argi__++);
	bool __ret__ = ImGui::IsMouseHoveringRect(r_min, r_max, clip);
	lua_pushboolean(L, __ret__);
	lua_pushnumber(L, r_min.x);
	lua_pushnumber(L, r_min.y);
	lua_pushnumber(L, r_max.x);
	lua_pushnumber(L, r_max.y);
	return 5;
};

//bool  IsMousePosValid(const ImVec2* mouse_pos);
//arg1	const ImVec2*	mouse_pos	=	 NULL
int cximgui_IsMousePosValid_1_v2p(lua_State* L) {
	int __argi__ = 0;
	ImVec2 mouse_pos;
	mouse_pos.x = (float)lua_tonumber(L, __argi__++);
	mouse_pos.y = (float)lua_tonumber(L, __argi__++);
	bool __ret__ = ImGui::IsMousePosValid(&mouse_pos);
	lua_pushboolean(L, __ret__);
	return 1;
};

//ImVec2  GetMousePos();
int cximgui_GetMousePos(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetMousePos();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetMousePosOnOpeningCurrentPopup();
int cximgui_GetMousePosOnOpeningCurrentPopup(lua_State* L) {
	ImVec2 __ret__ = ImGui::GetMousePosOnOpeningCurrentPopup();
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//ImVec2  GetMouseDragDelta(int button,float lock_threshold);
//arg1	int	button	=	 0
//arg2	float	lock_threshold	=	 -1.0f
int cximgui_GetMouseDragDelta_2_in(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	float lock_threshold = (float)lua_tonumber(L, __argi__++);
	ImVec2 __ret__ = ImGui::GetMouseDragDelta(button, lock_threshold);
	lua_pushnumber(L, __ret__.x);
	lua_pushnumber(L, __ret__.y);
	return 2;
};

//void  ResetMouseDragDelta(int button);
//arg1	int	button	=	 0
int cximgui_ResetMouseDragDelta_1_i(lua_State* L) {
	int __argi__ = 0;
	int button = (int)lua_tointeger(L, __argi__++);
	ImGui::ResetMouseDragDelta(button);
	return 0;
};

//ImGuiMouseCursor  GetMouseCursor();
int cximgui_GetMouseCursor(lua_State* L) {
	ImGuiMouseCursor __ret__ = ImGui::GetMouseCursor();
	lua_pushinteger(L, __ret__);
	return 1;
};

//void  SetMouseCursor(ImGuiMouseCursor type);
//arg1	ImGuiMouseCursor	type	=	
int cximgui_SetMouseCursor_1_i(lua_State* L) {
	int __argi__ = 0;
	ImGuiMouseCursor type = (ImGuiMouseCursor)lua_tointeger(L, __argi__++);
	ImGui::SetMouseCursor(type);
	return 0;
};

//void  CaptureKeyboardFromApp(bool capture);
//arg1	bool	capture	=	 true
int cximgui_CaptureKeyboardFromApp_1_b(lua_State* L) {
	int __argi__ = 0;
	bool capture = lua_toboolean(L, __argi__++);
	ImGui::CaptureKeyboardFromApp(capture);
	return 0;
};

//void  CaptureMouseFromApp(bool capture);
//arg1	bool	capture	=	 true
int cximgui_CaptureMouseFromApp_1_b(lua_State* L) {
	int __argi__ = 0;
	bool capture = lua_toboolean(L, __argi__++);
	ImGui::CaptureMouseFromApp(capture);
	return 0;
};

//const char*  GetClipboardText();
int cximgui_GetClipboardText(lua_State* L) {
	const char* __ret__ = ImGui::GetClipboardText();
	lua_pushstring(L, __ret__);
	return 1;
};

//void  SetClipboardText(const char* text);
//arg1	const char*	text	=	
int cximgui_SetClipboardText_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* text = lua_tostring(L, __argi__++);
	ImGui::SetClipboardText(text);
	return 0;
};

//void  LoadIniSettingsFromDisk(const char* ini_filename);
//arg1	const char*	ini_filename	=	
int cximgui_LoadIniSettingsFromDisk_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* ini_filename = lua_tostring(L, __argi__++);
	ImGui::LoadIniSettingsFromDisk(ini_filename);
	return 0;
};

//void  LoadIniSettingsFromMemory(const char* ini_data,size_t ini_size);
//arg1	const char*	ini_data	=	
//arg2	size_t	ini_size	=	0
int cximgui_LoadIniSettingsFromMemory_2_si(lua_State* L) {
	int __argi__ = 0;
	const char* ini_data = lua_tostring(L, __argi__++);
	size_t ini_size = (size_t)lua_tointeger(L, __argi__++);
	ImGui::LoadIniSettingsFromMemory(ini_data, ini_size);
	return 0;
};

//void  SaveIniSettingsToDisk(const char* ini_filename);
//arg1	const char*	ini_filename	=	
int cximgui_SaveIniSettingsToDisk_1_s(lua_State* L) {
	int __argi__ = 0;
	const char* ini_filename = lua_tostring(L, __argi__++);
	ImGui::SaveIniSettingsToDisk(ini_filename);
	return 0;
};

//const char*  SaveIniSettingsToMemory(size_t* out_ini_size);
//arg1	size_t*	out_ini_size	=	 NULL
int cximgui_SaveIniSettingsToMemory_1_ip(lua_State* L) {
	int __argi__ = 0;
	size_t out_ini_size = (size_t)lua_tointeger(L, __argi__++);
	const char* __ret__ = ImGui::SaveIniSettingsToMemory(&out_ini_size);
	lua_pushstring(L, __ret__);
	return 1;
};

//void  SetAllocatorFunctions(void* @1@2,void @3@4,void* user_data);
//arg1	void*	@1@2	=	
//arg2	void	@3@4	=	
//arg3	void*	user_data	=	 NULL
//UnSupported SetAllocatorFunctions
//void*  MemAlloc(size_t size);
//arg1	size_t	size	=	
int cximgui_MemAlloc_1_i(lua_State* L) {
	int __argi__ = 0;
	size_t size = (size_t)lua_tointeger(L, __argi__++);
	ImGui::MemAlloc(size);
	return 0;
};

//void  MemFree(void* ptr);
//arg1	void*	ptr	=	
//UnSupported MemFree
//ImGuiPlatformIO&  GetPlatformIO();
int cximgui_GetPlatformIO(lua_State* L) {
	ImGuiPlatformIO& __ret__ = ImGui::GetPlatformIO();
	return 0;
};

//ImGuiViewport*  GetMainViewport();
int cximgui_GetMainViewport(lua_State* L) {
	ImGuiViewport* __ret__ = ImGui::GetMainViewport();
	return 0;
};

//void  UpdatePlatformWindows();
int cximgui_UpdatePlatformWindows(lua_State* L) {
	ImGui::UpdatePlatformWindows();
	return 0;
};

//void  RenderPlatformWindowsDefault(void* platform_arg,void* renderer_arg);
//arg1	void*	platform_arg	=	 NULL
//arg2	void*	renderer_arg	=	 NULL
//UnSupported RenderPlatformWindowsDefault
//void  DestroyPlatformWindows();
int cximgui_DestroyPlatformWindows(lua_State* L) {
	ImGui::DestroyPlatformWindows();
	return 0;
};

//ImGuiViewport*  FindViewportByPlatformHandle(void* platform_handle);
//arg1	void*	platform_handle	=	
//UnSupported FindViewportByPlatformHandle

//total func	358	unSupported	84
