#ifndef PLAYER_H
#define PLAYER_H

#include "Math.hpp"
#include "Camera.h"
#include "Window.h"
#include "World.h"
#include <vector>

namespace CC {
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
		float baseSpeed = 16;				// base speed. always the same
		float speed = baseSpeed;			// modifier in determining player movement speed
		float gravity = -9.81f * speed;		
		float jumpHeight = 1.5f;			// 1.5 blocks high
		float mouseLookBound = 90.0f;		// cant look past straight up or straight down
		float mouseSensitivity = 0.15f;

		Vec4 moveDirection;					// determines horizontal movement
		float fallingSpeed = 0;				// determines vertical movement

		float width = 0.25f;
		float height = 1.75f;

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
		void Collide(const Vec4& vel);
		bool WouldCollide(Vec4 blockPosition);

		void Player::PrintDebugInfo();
	};
}
#endif // !PLAYER_H