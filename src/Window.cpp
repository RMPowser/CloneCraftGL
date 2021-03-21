#include "Window.h"

void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	using namespace std;

	cout << "--------------glDebugCallback-start--------------" << endl;
	cout << "message: " << message << endl;
	cout << "type: ";
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			cout << "OTHER";
			break;
	}
	cout << endl;

	cout << "id: " << id << endl;
	cout << "severity: ";
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			cout << "HIGH";
			break;
	}
	cout << endl;
	cout << "--------------glDebugCallback-end----------------" << endl;
}

namespace CC {
	Window::Window() {
		if (!glfwInit()) {
			throw std::exception("failed to init glfw!");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_RESIZABLE, true);

#ifndef NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif // !NDEBUG


		window = glfwCreateWindow(800, 600, "CloneCraftGL ", 0, 0);
		glfwMakeContextCurrent(window);

		glfwSwapInterval(1); // vsync

		if (glewInit() != GLEW_OK) {
			throw std::exception("failed to init glew!");
		}

		GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
#ifdef _WIN32
		uwh.window = glfwGetWin32Window(window);
#elif defined(__linux__)
		uwh.window = glfwGetX11Window(window);
#endif

		if (-inputHandler.Create(uwh)) {
			throw std::exception("inputHandler creation failed!");
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageCallback(glDebugCallback, nullptr);
		GLuint bogus;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &bogus, true);

		glClearColor((70.0f / 255), (160.0f / 255), (255.0f / 255), (255.0f / 255));
	}

	Window::~Window() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool Window::IsFocus() {
		return glfwGetWindowAttrib(window, GLFW_FOCUSED);
	}

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(window);
	}

	void Window::SetWindowName(const char* newName) {
		glfwSetWindowTitle(window, newName);
	}

	float Window::GetAspectRatio() {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		return (float)width / (float)height;
	}

	void Window::PollInput() {
		static auto getKey = [&](int keyCode) -> bool {
			float outstate;
			inputHandler.GetState(keyCode, outstate);
			return outstate > 0.0f ? true : false;
		};

		keys[G_KEY_W] = getKey(G_KEY_W);
		keys[G_KEY_A] = getKey(G_KEY_A);
		keys[G_KEY_S] = getKey(G_KEY_S);
		keys[G_KEY_D] = getKey(G_KEY_D);
		keys[G_KEY_I] = getKey(G_KEY_I);
		keys[G_KEY_J] = getKey(G_KEY_J);
		keys[G_KEY_K] = getKey(G_KEY_K);
		keys[G_KEY_L] = getKey(G_KEY_L);
		keys[G_KEY_O] = getKey(G_KEY_O);
		keys[G_KEY_U] = getKey(G_KEY_U);
		keys[G_KEY_F] = getKey(G_KEY_F);
		keys[G_KEY_SPACE] = getKey(G_KEY_SPACE);
		keys[G_KEY_CONTROL] = getKey(G_KEY_CONTROL);
		keys[G_KEY_LEFTSHIFT] = getKey(G_KEY_LEFTSHIFT);
		keys[G_BUTTON_LEFT] = getKey(G_BUTTON_LEFT);
		keys[G_BUTTON_RIGHT] = getKey(G_BUTTON_RIGHT);
		keys[G_MOUSE_SCROLL_UP] = getKey(G_MOUSE_SCROLL_UP);
		keys[G_MOUSE_SCROLL_DOWN] = getKey(G_MOUSE_SCROLL_DOWN);

		// set axis
		player_sideAxis = 0;
		player_fwdAxis = 0;
		player_upAxis = 0;
		if (keys[G_KEY_A]) { player_sideAxis -= 1; }
		if (keys[G_KEY_D]) { player_sideAxis += 1; }
		if (keys[G_KEY_W]) { player_fwdAxis -= 1; }
		if (keys[G_KEY_S]) { player_fwdAxis += 1; }
		if (keys[G_KEY_CONTROL]) { player_upAxis -= 1; }
		if (keys[G_KEY_SPACE]) { player_upAxis += 1; }

		light_sideAxis = 0;
		light_fwdAxis = 0;
		light_upAxis = 0;
		if (keys[G_KEY_J]) { light_sideAxis -= 1; }
		if (keys[G_KEY_L]) { light_sideAxis += 1; }
		if (keys[G_KEY_I]) { light_fwdAxis -= 1; }
		if (keys[G_KEY_K]) { light_fwdAxis += 1; }
		if (keys[G_KEY_O]) { light_upAxis -= 1; }
		if (keys[G_KEY_U]) { light_upAxis += 1; }


		float x, y;
		GW::GReturn result = inputHandler.GetMouseDelta(x, y);
		if (G_PASS(result) && result != GW::GReturn::REDUNDANT) {
			mouseDelta[0] = x;
			mouseDelta[1] = y;
		}
		else {
			mouseDelta[0] = mouseDelta[1] = 0;
		}
	}
}