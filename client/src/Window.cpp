#include "Window.h"

#include "global.h"
#include "core/Shader.h"
#include "file_system.h"
#include "InputManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Time/Time.h"
#include "script_system.h"
#include "InputManager.h"
#include "ne_wrapper.h"
#include "debugger.h"

static const float MS_PER_UPDATE = 1000 / 60.f / 1000;


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

Window::Window()
:m_Width(0),m_Height(0),m_FPS(MS_PER_UPDATE)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);	
	
	auto floatConfig = script_system_get_config("window_float");
	glfwWindowHint(GLFW_FLOATING, strcmp(floatConfig , "1")==0);
}

Window::~Window()
{
	
}

void Window::Init(int w,int h)
{
	m_Width = w;
	m_Height = h;
	
	m_pWindow = glfwCreateWindow(m_Width, m_Height, WINDOW_TITLE, nullptr, nullptr);
	if (m_pWindow == nullptr)
	{
		LOG_ERR("glfwCreateWindow failed!");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1); // Enable vsync


	glfwSetFramebufferSizeCallback(m_pWindow, glfw_framebuffer_size_callback);
	glfwSetCursorPosCallback(m_pWindow, glfw_mouse_callback);
	glfwSetMouseButtonCallback(m_pWindow, glfw_button_callback);
	glfwSetKeyCallback(m_pWindow, glfw_key_callback);
	glfwSetScrollCallback(m_pWindow, glfw_scroll_callback);
	glfwSetCharCallback(m_pWindow, glfw_character_callback);
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glViewport(0, 0, m_Width, m_Height);

	glewExperimental = GL_TRUE;
	glewInit();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
																//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
																//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
																//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
																// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(m_pWindow,false);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
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
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		
		auto now = glfwGetTime();
		m_FPS = (float)(now - previous);
		previous = now;
		
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		script_system_update();
		script_system_draw();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(m_pWindow);
		glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		glfwMakeContextCurrent(m_pWindow);
		glfwSwapBuffers(m_pWindow);
    }
	
	script_system_deinit();
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_pWindow);
    glfwTerminate();

	auto* ne_thread = ne_support_get_thread();
	delete ne_thread;
	ne_thread = nullptr;
}

float Window::GetDeltaTime()
{
	return m_FPS;
}

// เล

float Window::GetDeltaTimeMilliseconds()
{
	return m_FPS *1000.f;
}

void Window::OnFrameBufferSizeCallback(int width, int height)
{
	m_Width = width;
	m_Height = height;

	glViewport(0, 0, m_Width, m_Height);
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
void luaopen_window(lua_State* L)
{
	script_system_register_function(L, window_system_init);
	script_system_register_function(L, window_system_show);
	script_system_register_function(L, window_system_set_floating);
}
