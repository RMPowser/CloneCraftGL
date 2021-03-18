#ifndef WINDOW_H
#define WINDOW_H

#include "OpenGL.hpp"
#include "Gateware.hpp"
#include "Math.hpp"
#include <unordered_map>

namespace CC {
	class Window {
		// window
	public:
		GLFWwindow* window;
		Window();
		~Window();
		bool IsFocus();
		bool ShouldClose();
		void SetWindowName(const char* newName);
		float GetAspectRatio();

		// inputHandler
	private:
		GW::INPUT::GInput inputHandler;
		std::unordered_map<int, bool> keys;
		Vec2 clientCenter { 0, 0 };
		Vec2 mouseDelta { 0, 0 };
		bool flyToggle = false;
		bool firstMouse = true;
		float horizontalAxis = 0; // left/right axis
		float forwardAxis = 0; // forward/backward axis
		float verticalAxis = 0; // up/down axis

	public:
		inline bool GetKey(int keyCode) { return keys[keyCode]; }
		inline float GetInputAxisSide() { return horizontalAxis; }
		inline float GetInputAxisUp() { return verticalAxis; }
		inline float GetInputAxisFwd() { return forwardAxis; }
		inline const Vec2& GetMouseDelta() const { return mouseDelta; }

		void PollInput();
	};
}

#endif // WINDOW_H