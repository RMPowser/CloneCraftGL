#pragma once

#include "OpenGL.hpp"
#include "Gateware.hpp"
#include "Math.hpp"
#include <unordered_map>

class Window {
	friend class Renderer;
	friend class Camera;
	// window
private:
	GLFWwindow* glfwWindow;
public:
	Window();
	~Window();
	bool IsFocus() const;
	bool ShouldClose() const;
	void SetWindowName(const char* newName);
	float GetAspectRatio() const;
	void Update();

	// inputHandler
private:
	GW::INPUT::GInput inputHandler;
	std::unordered_map<int, bool> keys;
	Vec2 mouseDelta { 0, 0 };

public:
	bool GetKey(int keyCode);
	inline const float GetMouseDeltaX() const { return mouseDelta.x; }
	inline const float GetMouseDeltaY() const { return mouseDelta.z; }

};