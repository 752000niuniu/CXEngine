#include <window.h>

#include <file_system.h>
#include <imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <script_system.h>
#include <file_loading.h>
#include <logger.h>
#include "cxlua.h"

static int m_WindowWidth;
static int m_WindowHeight;
static const float MS_PER_UPDATE = 1000 / 60.f / 1000;
#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 600
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description) {
	cxlog_err("Error: %d %s\n", error, description);
}

static GLFWwindow* s_pWindow = nullptr;
bool iw_should_close()
{
	return glfwWindowShouldClose(s_pWindow);
}

void iw_begin_render()
{
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void iw_end_render()
{
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(s_pWindow, &display_w, &display_h);
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

	glfwSwapBuffers(s_pWindow);
}

void iw_init() {
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
	std::string floatConfig = command_arg_opt_str("window_float", "0");
	glfwWindowHint(GLFW_FLOATING, floatConfig == "1");

	s_pWindow = glfwCreateWindow(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, "CXLUAX", nullptr, nullptr);
	if (s_pWindow == nullptr)
	{
		cxlog_err("glfwCreateWindow failed!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwGetWindowSize(s_pWindow, &m_WindowWidth, &m_WindowHeight);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW = 0, monitorH = 0;
	glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorW, &monitorH);
	glfwSetWindowPos(s_pWindow, (monitorW - m_WindowWidth) / 2, (monitorH - m_WindowHeight) / 2);

	glfwMakeContextCurrent(s_pWindow);
	glfwSwapInterval(1);// Enable vsync

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

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(s_pWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glfwSetInputMode(s_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetWindowOpacity(s_pWindow, 1.0f);
}

void iw_deinit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(s_pWindow);
	glfwTerminate();

}
void iw_set_font(const char* path) {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(path, 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
}

void luaopen_window(lua_State* L)
{
	script_system_register_function(L, iw_init);
	script_system_register_function(L, iw_deinit);
	script_system_register_function(L, iw_should_close);
	script_system_register_function(L, iw_begin_render);
	script_system_register_function(L, iw_end_render);

	script_system_register_function(L, iw_set_font);
}
