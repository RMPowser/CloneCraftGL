#include "Window.h"
#include "OpenGL.hpp"

void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::cout << "--------------glDebugCallback-start--------------" << std::endl;
	std::cout << "message: " << message << std::endl;
	std::cout << "type: ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
	}
	std::cout << std::endl;

	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
	}
	std::cout << std::endl;
	std::cout << "--------------glDebugCallback-end----------------" << std::endl;
}

Window::Window() {
	if (!glfwInit()) {
		throw std::exception("failed to init glfw!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);

#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


	glfwWindow = glfwCreateWindow(800, 600, "CloneCraftGL ", 0, 0);
	glfwMakeContextCurrent(glfwWindow);


	if (glewInit() != GLEW_OK) {
		throw std::exception("failed to init glew!");
	}

	GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
#ifdef _WIN32
	uwh.window = glfwGetWin32Window(glfwWindow);
#elif defined(__linux__)
	uwh.window = glfwGetX11Window(window);
#endif

	if (-inputHandler.Create(uwh)) {
		throw std::exception("inputHandler creation failed!");
	}

	glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback(glDebugCallback, nullptr);
	GLuint bogus;
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &bogus, true);
}

Window::~Window() {
	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
}

bool Window::IsFocus() const {
	return glfwGetWindowAttrib(glfwWindow, GLFW_FOCUSED);
}

bool Window::ShouldClose() const {
	return glfwWindowShouldClose(glfwWindow);
}

void Window::SetWindowName(const char* newName) {
	glfwSetWindowTitle(glfwWindow, newName);
}

float Window::GetAspectRatio() const {
	int width, height;
	glfwGetFramebufferSize(glfwWindow, &width, &height);
	return (float)width / (float)height;
}

void Window::Update() {
	glfwPollEvents();

	float x, y;
	GW::GReturn result = inputHandler.GetMouseDelta(x, y);
	if (G_PASS(result) && result != GW::GReturn::REDUNDANT) {
		mouseDelta.x = x;
		mouseDelta.z = y;
	}
	else {
		mouseDelta = { 0, 0 };
	}
}

bool Window::GetKey(int GW_keyCode) {
	float outstate;
	inputHandler.GetState(GW_keyCode, outstate);
	return outstate > 0.0f ? true : false;
}