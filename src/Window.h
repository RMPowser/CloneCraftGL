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
		float player_sideAxis = 0; // left/right axis
		float player_fwdAxis = 0; // forward/backward axis
		float player_upAxis = 0; // up/down axis
		float light_sideAxis = 0;
		float light_fwdAxis = 0;
		float light_upAxis = 0;

	public:
		inline bool GetKey(int keyCode) { return keys[keyCode]; }
		inline float GetInputPlayerAxisSide() { return player_sideAxis; }
		inline float GetInputPlayerAxisFwd() { return player_fwdAxis; }
		inline float GetInputPlayerAxisUp() { return player_upAxis; }
		inline float GetInputLightAxisSide() { return light_sideAxis; }
		inline float GetInputLightAxisFwd() { return light_fwdAxis; }
		inline float GetInputLightAxisUp() { return light_upAxis; }
		inline const Vec2& GetMouseDelta() const { return mouseDelta; }

		void PollInput();
	};
}

#endif // WINDOW_H