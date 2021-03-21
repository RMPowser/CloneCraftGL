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
	std::string glVersion = (const char*)glGetString(GL_VERSION);
	std::string glVersionInfo = "OpenGL Driver Version " + glVersion + "\n";
	printf(glVersionInfo.c_str());

	//CC::Window window;
	CC::Renderer renderer;
	//CC::Player player;
	//CC::Camera camera;
	//CC::World world;

	double dt = 0;

	while (!renderer.GetWindow().ShouldClose()) {
		dt = GetDeltaTime();
		renderer.Update(dt);
		renderer.Draw(dt);
	}

	return 0;
}