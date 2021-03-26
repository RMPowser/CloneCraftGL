#include "Player.h"

Player::Player(Window& window, World& world, Camera& cam) : window(window), world(world), camera(cam) {
	mMat.Position() = Vec4(camera.position, 1); // set spawn
}

void Player::Update(float dt) {
	// set axis
	sideAxis = 0;
	fwdAxis = 0;
	upAxis = 0;
	if (window.GetKey(G_KEY_A)) { sideAxis -= 1; }
	if (window.GetKey(G_KEY_D)) { sideAxis += 1; }
	if (window.GetKey(G_KEY_W)) { fwdAxis -= 1; }
	if (window.GetKey(G_KEY_S)) { fwdAxis += 1; }
	if (window.GetKey(G_KEY_CONTROL)) { upAxis -= 1; }
	if (window.GetKey(G_KEY_SPACE)) { upAxis += 1; }

	// --- speed modifier --- //
	if (window.GetKey(G_KEY_LEFTSHIFT)) {
		speed = baseSpeed * 1.5f;
	}
	else {
		speed = baseSpeed;
	}

	// --- mouse scroll --- //
	if (window.GetKey(G_MOUSE_SCROLL_UP)) {
		int fov = camera.GetFOV();
		camera.SetFOV(--fov, window.GetAspectRatio());
	}

	if (window.GetKey(G_MOUSE_SCROLL_DOWN)) {
		int fov = camera.GetFOV();
		camera.SetFOV(++fov, window.GetAspectRatio());
	}

	// toggle flying
	if (window.GetKey(G_KEY_F) && flyToggle) {
		isFlying = !isFlying;
		flyToggle = false;
	}
	else if (!window.GetKey(G_KEY_F)) {
		flyToggle = true;
	}


	// break block
	/*if (window.GetKey(G_BUTTON_LEFT) && canPlaceBlock) {
		auto& world = AppGlobals::world;

		for (Ray ray(camera.position, camera.rotation); ray.GetLength() <= buildRange; ray.Step(0.001f)) {
			auto rayEnd = ray.GetEnd();
			auto block = world.getBlock(rayEnd);

			if (block != BlockType::Air) {
				if (world.setBlock(BlockType::Air, rayEnd)) {
					Vec4 xz = World::getChunkXZ(rayEnd);
					world.updateChunk(world.getChunk(xz)->position);
					world.updateChunk(world.getChunk(Vec4(xz.x + 1, 0.f, xz.z, 0.f))->position);
					world.updateChunk(world.getChunk(Vec4(xz.x - 1, 0.f, xz.z, 0.f))->position);
					world.updateChunk(world.getChunk(Vec4(xz.x, 0.f, xz.z + 1, 0.f))->position);
					world.updateChunk(world.getChunk(Vec4(xz.x, 0.f, xz.z - 1, 0.f))->position);
					break;
				}
				else {
					__debugbreak();
					throw new std::runtime_error("unable to destroy block!");
				}
			}
		}

		canPlaceBlock = false;
	}
	else if (!controller.keys[G_BUTTON_LEFT]) {
		canPlaceBlock = true;
	}*/


	// place block
	/*if (window.GetKey(G_BUTTON_RIGHT) && canBreakBlock) {
		Vec4 lastRayPosition { 0, 0, 0, 0 };
		auto& world = AppGlobals::world;

		for (Ray ray(camera.position, camera.rotation); ray.GetLength() <= buildRange; ray.Step(0.001f)) {
			auto rayEnd = ray.GetEnd();
			auto block = world.getBlock(rayEnd);
			auto blockPosition = lastRayPosition.AsInt();

			if (block != BlockType::Air) {
				if (!wouldCollide(blockPosition)) {
					if (world.setBlock(BlockType::Grass, lastRayPosition)) {
						Vec4 xz = World::getChunkXZ(lastRayPosition);
						world.updateChunk(world.getChunk(xz)->position);
						break;
					}
				}
				else {
					break;
				}
			}
			lastRayPosition = ray.GetEnd();
		}
		canBreakBlock = false;
	}
	else if (!controller.keys[G_BUTTON_RIGHT]) {
		canBreakBlock = true;
	}*/




	// --- jumping --- //
	if (window.GetKey(G_KEY_SPACE) && canJump && !isFlying && onGround) {
		// fall up lol
		fallingSpeed = sqrtf(jumpHeight * -2.0f * gravity);
		canJump = false;
	}
	else if (!window.GetKey(G_KEY_SPACE)) {
		canJump = true;
	}


	// --- gravity --- //
	if (!isFlying) {
		fallingSpeed += gravity * dt;
	}
	else {
		fallingSpeed = upAxis * speed;
	}


	// --- world wrap --- //
	if (mMat.Position().y < -10 && !isFlying) {
		mMat.Position().y = 260;
	}

	// Collide() will set onGround
	onGround = false;


	// --- horizontal movement --- //
	moveDirection = Normalize(camera.GetRightAxis() * sideAxis + camera.GetForwardAxis() * fwdAxis);
	mMat.Position().x += moveDirection.x * speed * dt;
	Collide(Vec4(moveDirection.x * speed * dt, 0.f, 0.f, 0.f));
	mMat.Position().z += moveDirection.z * speed * dt;
	Collide(Vec4(0.0f, 0.f, moveDirection.z * speed * dt, 0.f));

	// --- vertical movement --- //
	mMat.Position().y += fallingSpeed * dt;
	Collide(Vec4(0.f, fallingSpeed, 0.f, 0.f));

	// --- mouse look --- //
	camera.rotation.x += window.GetMouseDeltaY() * mouseSensitivity;
	camera.rotation.y += window.GetMouseDeltaX() * mouseSensitivity;

	if (camera.rotation.x > mouseLookBound) {
		camera.rotation.x = mouseLookBound;
	}
	else if (camera.rotation.x < -mouseLookBound) {
		camera.rotation.x = -mouseLookBound;
	}

	if (camera.rotation.y >= 360) {
		camera.rotation.y = 0;
	}
	else if (camera.rotation.y < 0) {
		camera.rotation.y = 360;
	}




	camera.position = mMat.Position();
	camera.position.y += height;
}

void Player::Collide(const Vec4& vel) {
	float xMin = mMat.Position().x - width;
	float xMax = mMat.Position().x + width;
	float yMin = mMat.Position().y;
	float yMax = mMat.Position().y + height;
	float zMin = mMat.Position().z - width;
	float zMax = mMat.Position().z + width;

	for (int x = xMin; x < xMax; x++) {
		for (int y = yMin; y < yMax; y++) {
			for (int z = zMin; z < zMax; z++) {
				auto block = world.GetBlock(Vec4(x, y, z, 0));

				if (world.GetBlockDataFor(block).isCollidable) {
					if (vel.y > 0) {
						mMat.Position().y = y - height;
						fallingSpeed = 0;
					}
					else if (vel.y < 0) {
						onGround = true;
						mMat.Position().y = y + 1;
						fallingSpeed = 0;
					}

					if (vel.x > 0) {
						mMat.Position().x = x - width;
					}
					else if (vel.x < 0) {
						mMat.Position().x = x + width + 1;
					}

					if (vel.z > 0) {
						mMat.Position().z = z - width;
					}
					else if (vel.z < 0) {
						mMat.Position().z = z + width + 1;
					}
				}
			}
		}
	}
}

// This function is for checking if a block being placed would collide with the player.
bool Player::WouldCollide(Vec4 blockPosition) {
	float xMin = mMat.Position().x - width;
	float xMax = mMat.Position().x + width;
	float yMin = mMat.Position().y;
	float yMax = mMat.Position().y + height;
	float zMin = mMat.Position().z - width;
	float zMax = mMat.Position().z + width;

	for (int x = xMin; x < xMax; x++) {
		for (int y = yMin; y < yMax; y++) {
			for (int z = zMin; z < zMax; z++) {
				if (blockPosition == Vec4(x, y, z, 0)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Player::PrintDebugInfo(float dt) {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { 0, 1 };
	SetConsoleCursorPosition(hStdout, position);

	printf(
R"(Player Info										
position:		x: %2f		y: %2f		z: %2f		
camera:			x: %2f		y: %2f		z: %2f		
rotationCam: 	x: %2f		y: %2f		z: %2f		
FOV: %2f											
FPS: %2f											

		)", mMat.Position().x, mMat.Position().y, mMat.Position().z,
		camera.position.x, camera.position.y, camera.position.z,
		camera.rotation.x, camera.rotation.y, camera.rotation.z,
		camera.GetFOV(),
		1 / dt);
}