#include "window.h"

#include "graphics/shader.h"
#include "file_system.h"
#include "input_manager.h"
#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "time/time.h"
#include <script_system.h>
#include "input_manager.h"
#include "file_loading.h"
#include "scene/scene_manager.h"
#include "logger.h"
#include "cxlua.h"
#include "sprite_renderer.h"
#include <graphics/ui_renderer.h>

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 600
static const float MS_PER_UPDATE = 1000 / 60.f / 1000;

int m_Width;
int m_Height;
int m_WindowWidth;
int m_WindowHeight;
float m_FPS;
GLFWwindow* m_pWindow;


unsigned int m_Fbo;
unsigned int m_Rbo;
unsigned int m_TextureColor;

GLFWwindow* Window::GetGLFWwindow() { return m_pWindow; };
int Window::GetWidth() { return m_Width; };
int Window::GetHeight() { return m_Height; };
int Window::GetWindowWidth() { return m_WindowWidth; };
int Window::GetWindowHeight() { return m_WindowHeight; };
float Window::GetCenterX() { return GetWidth() / 2.f; }
float Window::GetCenterY() { return GetHeight() / 2.f; }
float Window::GetFPS() { return m_FPS; }

int Window::GetRenderTexture() { return m_TextureColor; }
int Window::GetFrameBuffer() { return m_Fbo; }
static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WINDOW_INSTANCE->OnFrameBufferSizeCallback(width, height);
}

static void glfw_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	INPUT_MANAGER_INSTANCE->MouseButtonCallback(window, button, action, mods);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);
	INPUT_MANAGER_INSTANCE->KeyCallbackFunc(window, key, scancode, action, mode);
}
static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	INPUT_MANAGER_INSTANCE->ScrollCallbackFunc(window, (float)xoffset, (float)yoffset);
}

static void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	INPUT_MANAGER_INSTANCE->MouseCallbackFunc(window, (float)xpos, (float)ypos);
}

static void glfw_character_callback(GLFWwindow* window, unsigned int charcode)
{
	ImGui_ImplGlfw_CharCallback(window, charcode);
	INPUT_MANAGER_INSTANCE->CharacterInputCallback(window, charcode);
}

static void glfw_error_callback(int error, const char* description) {
	cxlog_err("Error: %d %s\n", error, description);
}

Window::Window()
{
	m_Width = 0;
	m_Height = 0;
	m_FPS = MS_PER_UPDATE;
	m_pWindow = nullptr;
}

Window::~Window() {

}

void Window::Init(int w, int h)
{
	if (!glfwInit()) {
		cxlog_err("glfwInit error!");
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(glfw_error_callback);

#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	std::string floatConfig = command_arg_opt_str("window_float", "0");;
	glfwWindowHint(GLFW_FLOATING, floatConfig == "1");

	m_pWindow = glfwCreateWindow(w, h, "SimpleEngine", nullptr, nullptr);
	if (m_pWindow == nullptr)
	{
		cxlog_err("glfwCreateWindow failed!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwGetWindowSize(m_pWindow, &m_WindowWidth, &m_WindowHeight);
	m_Width = m_WindowWidth;
	m_Height = m_WindowHeight;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW = 0, monitorH = 0;
	glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorW, &monitorH);
	glfwSetWindowPos(m_pWindow, (monitorW - m_WindowWidth) / 2, (monitorH - m_WindowHeight) / 2);

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cxlog_err("glewInit error! %s\n", glewGetErrorString(err));
		return;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_pWindow, false);
	ImGui_ImplOpenGL3_Init(glsl_version);


	glfwSetFramebufferSizeCallback(m_pWindow, glfw_framebuffer_size_callback);
	glfwSetCursorPosCallback(m_pWindow, glfw_mouse_callback);
	glfwSetMouseButtonCallback(m_pWindow, glfw_button_callback);
	glfwSetKeyCallback(m_pWindow, glfw_key_callback);
	glfwSetScrollCallback(m_pWindow, glfw_scroll_callback);
	glfwSetCharCallback(m_pWindow, glfw_character_callback);
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowOpacity(m_pWindow, 1.0f);


	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	glGenTextures(1, &m_TextureColor);
	glBindTexture(GL_TEXTURE_2D, m_TextureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColor, 0);

	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cxlog_err("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	UIRenderer::GetInstance();
}

void Window::Destroy()
{
	script_system_deinit();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();

	auto* ne_thread = file_loading_thread();
	delete ne_thread;
	ne_thread = nullptr;
}

void Window::Show()
{
	script_system_init();
	double previous = glfwGetTime();
	double lag = 0;
	double delta = 0;
	ImGuiIO& io = ImGui::GetIO();
	while (!glfwWindowShouldClose(m_pWindow))
	{
		auto now = glfwGetTime();
		m_FPS = (float)(now - previous);
		previous = now;

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		script_system_update();
		script_system_draw();

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(m_pWindow);
	}

	Destroy();
}

float Window::GetDeltaTime()
{
	return m_FPS;
}


float Window::GetDeltaTimeMilliseconds()
{
	return m_FPS * 1000.f;
}

void Window::OnFrameBufferSizeCallback(int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;
	m_Width = width;
	m_Height = height;
	SpriteRenderer::GetInstance()->UpdateProjection();

	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	int screenWidth = WINDOW_INSTANCE->GetWidth();
	int screenHeight = WINDOW_INSTANCE->GetHeight();
	glGenTextures(1, &m_TextureColor);
	glBindTexture(GL_TEXTURE_2D, m_TextureColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColor, 0);

	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cxlog_err("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
}

void iw_set_font(const char* path) {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(path, 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
}

void window_system_init(int w, int h)
{
	WINDOW_INSTANCE->Init(w, h);
}

void window_system_show()
{
	WINDOW_INSTANCE->Show();
}
void window_system_set_floating(int opt, int value)
{
	WINDOW_INSTANCE;
	glfwWindowHint(opt, value);
}

float window_system_get_dt()
{
	return WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
}

float window_system_get_fps()
{
	return WINDOW_INSTANCE->GetFPS();
}

int game_get_width() {
	return WINDOW_INSTANCE->GetWidth();
}
int game_get_height() {
	return WINDOW_INSTANCE->GetHeight();
}

bool iw_should_close()
{
	return glfwWindowShouldClose(WINDOW_INSTANCE->GetGLFWwindow());
}
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


void iw_deinit() {
	WINDOW_INSTANCE->Destroy();
}

void iw_function_to_select_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
	glDisable(GL_BLEND);
}

void iw_function_to_restore_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
	glEnable(GL_BLEND);
}

void iw_init() {
	WINDOW_INSTANCE->Init(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT);
}

int iw_render(lua_State* L)
{
	auto* win = WINDOW_INSTANCE->GetGLFWwindow();
	int gameWidth = WINDOW_INSTANCE->GetWidth();
	int gameHeight = WINDOW_INSTANCE->GetHeight();
	int display_w, display_h;
	glfwGetFramebufferSize(win, &display_w, &display_h);
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glViewport(0, 0, gameWidth, gameHeight);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	lua_pushvalue(L, 2);
	lua_pcall(L, 0, 0, 0);
	UIRenderer::GetInstance()->Begin();
	UIRenderer::GetInstance()->Draw();
	UIRenderer::GetInstance()->End();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(mainViewport->Pos);
	ImGui::SetNextWindowSize(mainViewport->Size);
	ImGui::SetNextWindowViewport(mainViewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
	ImGui::PopStyleVar();

	ImVec2 cursorPos = ImGui::GetCursorPos();
	auto cspos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddCallback(iw_function_to_select_shader_or_blend_state, nullptr);
	ImGui::GetWindowDrawList()->AddImage((void*)(uint64_t)m_TextureColor, cspos, ImVec2(cspos.x + gameWidth, cspos.y + gameHeight), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::GetWindowDrawList()->AddCallback(iw_function_to_restore_shader_or_blend_state, nullptr);
	ImGui::SetCursorPos(cursorPos);
	lua_pushvalue(L, 1);
	lua_pcall(L, 0, 0, 0);
	ImGui::End();


	ImGui::Render();
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
	glfwSwapBuffers(win);
	return 0;
}

void luaopen_window(lua_State* L)
{
	script_system_register_function(L, window_system_init);
	script_system_register_function(L, window_system_show);
	script_system_register_function(L, window_system_set_floating);
	script_system_register_function(L, window_system_get_dt);
	script_system_register_function(L, window_system_get_fps);

	script_system_register_function(L, game_get_width);
	script_system_register_function(L, game_get_height);


	script_system_register_function(L, iw_init);
	script_system_register_function(L, iw_deinit);
	script_system_register_function(L, iw_should_close);

	script_system_register_function(L, iw_set_font);


	script_system_register_luac_function(L, iw_render);

}
