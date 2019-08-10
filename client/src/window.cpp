#include "window.h"

#include "graphics/shader.h"
#include "file_system.h"
#include "input_manager.h"
#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "time/time.h"
#include "script_system.h"
#include "input_manager.h"
#include "file_loading.h"
#include "scene/scene_manager.h"
#include "logger.h"
static const float MS_PER_UPDATE = 1000 / 60.f / 1000;

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 600
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
	printf("Error: %s\n", description);
}


Window::Window()
	:m_Width(GAME_SCREEN_WIDTH), m_Height(GAME_SCREEN_HEIGHT), m_FPS(MS_PER_UPDATE), m_pWindow(nullptr){
	
}

Window::~Window() {
	
}

void Window::Init(int w,int h)
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
	std::string floatConfig = script_system_get_config("window_float");
	glfwWindowHint(GLFW_FLOATING, floatConfig == "1");

	m_pWindow = glfwCreateWindow(w, h, "SimpleEngine", nullptr, nullptr);
	if (m_pWindow == nullptr)
	{
		cxlog_err("glfwCreateWindow failed!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwGetWindowSize(m_pWindow, &m_WindowWidth, &m_WindowHeight);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW = 0, monitorH = 0;
	glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorW, &monitorH);
	glfwSetWindowPos(m_pWindow, (monitorW - m_WindowWidth) / 2, (monitorH - m_WindowHeight) / 2);

	glfwMakeContextCurrent(m_pWindow);
	GLenum err = glewInit();
	if (GLEW_OK !=err) {
		cxlog_err("glewInit error! %s\n", glewGetErrorString(err));
		return;
	}
	glfwSwapInterval(1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(m_pWindow, false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::StyleColorsDark();

	glfwSetFramebufferSizeCallback(m_pWindow, glfw_framebuffer_size_callback);
	glfwSetCursorPosCallback(m_pWindow, glfw_mouse_callback);
	glfwSetMouseButtonCallback(m_pWindow, glfw_button_callback);
	glfwSetKeyCallback(m_pWindow, glfw_key_callback);
	glfwSetScrollCallback(m_pWindow, glfw_scroll_callback);
	glfwSetCharCallback(m_pWindow, glfw_character_callback);
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
	
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(mainViewport->Pos);
		ImGui::SetNextWindowSize(mainViewport->Size);
		ImGui::SetNextWindowViewport(mainViewport->ID);

		ImGui::Begin("MainDock", NULL,  ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		int dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id);
		ImGui::End();

		script_system_update();
		script_system_draw();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(m_pWindow);
		glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		glfwMakeContextCurrent(m_pWindow);
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
    }
	
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

float Window::GetDeltaTime()
{
	return m_FPS;
}


float Window::GetDeltaTimeMilliseconds()
{
	return m_FPS *1000.f;
}

void Window::OnFrameBufferSizeCallback(int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;
	glViewport(0, 0, m_WindowWidth, m_WindowHeight);
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
int game_get_width(){
	return WINDOW_INSTANCE->GetWidth();
}
int game_get_height(){
	return WINDOW_INSTANCE->GetHeight();
}

void luaopen_window(lua_State* L)
{
	script_system_register_function(L, window_system_init);
	script_system_register_function(L, window_system_show);
	script_system_register_function(L, window_system_set_floating);
	script_system_register_function(L, window_system_get_dt);

	script_system_register_function(L, game_get_width);
	script_system_register_function(L, game_get_height);
}
