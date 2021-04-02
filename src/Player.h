#pragma once

#include "Math.hpp"
#include "Camera.h"
#include "Window.h"
#include "World.h"
#include <vector>

class Player {
private:
	Camera& camera;
	Window& window;
	World& world;

	float sideAxis = 0;
	float fwdAxis = 0;
	float upAxis = 0;
	bool flyToggle = true;				// allowed to toggle flying? used to stop the game from spam-toggling flying when you press F
	float buildRange = 5;				// how far you can reach
	float baseSpeed = 5;				// base speed. always the same
	float speed = baseSpeed;			// modifier in determining player movement speed
	float gravity = -9.81f * speed;
	float jumpHeight = 1.5f;			// 1.5 blocks high
	float mouseLookBound = 89.0f;		// cant look past straight up or straight down
	float mouseSensitivity = 0.15f;

	Vec3 moveDirection;					// determines horizontal movement
	float fallingSpeed = 0;				// determines vertical movement

	float width = 0.25f;
	float height = 1.80f;

public:
	Mat4 mMat;							// model matrix
	bool isFlying = false;
	bool onGround = false;
	bool canJump = true;
	bool canPlaceBlock = true;
	bool canBreakBlock = true;
	bool lastFlyingState = false;

	Player(Window& window, World& world, Camera& cam);

	void Update(float dt);
	void Collide(const Vec3& vel);
	bool WouldCollide(const Vec3& blockPosition);

	void PrintDebugInfo(float dt);
};