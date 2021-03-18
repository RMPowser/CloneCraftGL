#ifndef RENDERER_H
#define RENDERER_H

#include "Math.hpp"
#include "Gateware.hpp"
#include "Camera.h"
#include "Window.h"

#include <string>
#include <iostream>
#include <fstream>

#define numVAOs 1
#define numVBOs 2

namespace CC {
	class Renderer {
	private:
		Window window;
		Camera cam;

		Mat4 cubeMMat;
		Mat4 mvMat;
		Vec4 cubeP;

		GLuint renderingProgram;
		GLuint vao[numVAOs];
		GLuint vbo[numVBOs];
		GLuint textureAtlas;

		GLuint mvMatLocation, pMatLocation;
		float aspect;

		void CreatePMat();

		// utilities
		GLuint CreateShaderProgram(const char* vShaderCode, const char* fShaderCode);
		std::string ReadShaderSource(const char* filePath);
		void PrintShaderLog(GLuint shader);
		void PrintProgramLog(int prog);
		bool CheckOpenGLError();
		void SetupVertices();
		GLuint LoadTexture(const char* textureImagePath);

	public:

		Renderer();
		void Update(double dt);
		void Draw(double dt);
		inline Window& GetWindow() { return window; }
	};
}

#endif // RENDERER_H