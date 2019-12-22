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

static void glfw_error_callback(int error, const char* description) {
	cxlog_err("Error: %d %s\n", error, description);
}

static GLFWwindow *s_pWindow = nullptr;
bool iw_should_close()
{
	return glfwWindowShouldClose(s_pWindow);
}

void iw_begin_render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(mainViewport->Pos);
	ImGui::SetNextWindowSize(mainViewport->Size);
	ImGui::SetNextWindowViewport(mainViewport->ID);

	ImGui::Begin("MainDock", NULL, ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
	int dockspace_id = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspace_id);
	ImGui::End();
}

void iw_end_render()
{ 
	ImGui::Render();
	int display_w, display_h;
	glfwMakeContextCurrent(s_pWindow);
	glfwGetFramebufferSize(s_pWindow, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	glfwMakeContextCurrent(s_pWindow);
	glfwSwapBuffers(s_pWindow);
	glfwPollEvents();
}

void iw_init(){
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
	std::string floatConfig = command_arg_opt_str("window_float","0");
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
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cxlog_err("glewInit error! %s\n", glewGetErrorString(err));
		return;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

	glfwSwapInterval(1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(s_pWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::StyleColorsDark();

	glfwSetInputMode(s_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowOpacity(s_pWindow, 1.0f);
}

void iw_deinit(){
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
