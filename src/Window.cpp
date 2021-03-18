#include "Window.h"


namespace CC {
	Window::Window() {
		if (!glfwInit()) {
			throw std::exception("failed to init glfw!");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_RESIZABLE, true);

		window = glfwCreateWindow(800, 600, "CloneCraftGL ", 0, 0);
		glfwMakeContextCurrent(window);

		glfwSwapInterval(0); // vsync

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
		keys[G_KEY_F] = getKey(G_KEY_F);
		keys[G_KEY_SPACE] = getKey(G_KEY_SPACE);
		keys[G_KEY_CONTROL] = getKey(G_KEY_CONTROL);
		keys[G_KEY_LEFTSHIFT] = getKey(G_KEY_LEFTSHIFT);
		keys[G_BUTTON_LEFT] = getKey(G_BUTTON_LEFT);
		keys[G_BUTTON_RIGHT] = getKey(G_BUTTON_RIGHT);
		keys[G_MOUSE_SCROLL_UP] = getKey(G_MOUSE_SCROLL_UP);
		keys[G_MOUSE_SCROLL_DOWN] = getKey(G_MOUSE_SCROLL_DOWN);

		// set axis
		horizontalAxis = 0;
		forwardAxis = 0;
		verticalAxis = 0;
		if (keys[G_KEY_A]) { horizontalAxis -= 1; }
		if (keys[G_KEY_D]) { horizontalAxis += 1; }
		if (keys[G_KEY_W]) { forwardAxis -= 1; }
		if (keys[G_KEY_S]) { forwardAxis += 1; }
		if (keys[G_KEY_SPACE]) { verticalAxis += 1; }
		if (keys[G_KEY_CONTROL]) { verticalAxis -= 1; }


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