#include "Window.h"
#include "World.h"
#include "Player.h"
#include "Renderer.h"
#include "OpenGL.hpp"


double GetDeltaTime() {
	static auto startTime = std::chrono::high_resolution_clock::now();
	
	auto currentTime = std::chrono::high_resolution_clock::now();
	double time = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime).count();
	startTime = std::chrono::high_resolution_clock::now();

	return time;
}

int main() {
	Vec3 spawnLocation { 1000.0f, 160.0f, 1000.0f };

	Window window;

	std::string glVersion = (const char*)glGetString(GL_VERSION);
	std::string glVersionInfo = "OpenGL Driver Version " + glVersion + "\n";
	printf(glVersionInfo.c_str());

	Renderer renderer(window);
	Camera camera(spawnLocation, window);
	World world(camera, renderer);
	Player player(window, world, camera);

	double dt = 0;

	while (!window.ShouldClose()) {
			dt = GetDeltaTime();
			window.Update();

		if (window.IsFocus()) {
			// --- Update --- //
			player.Update(dt);
			//player.PrintDebugInfo(dt);
			camera.Update(window.GetAspectRatio());
			//world.PrintDebugInfo();
			//world.Update(); the world updates itself now

			// ---  Draw  --- //
			renderer.ClearScreen();

			world.Draw();

			renderer.SwapBuffers();
		}
	}

	return 0;
}