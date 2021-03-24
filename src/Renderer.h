#ifndef RENDERER_H
#define RENDERER_H

#include "Math.hpp"
#include "Gateware.hpp"
#include "Camera.h"
#include "Window.h"
#include "Model.h"
#include "Light.h"
#include "ShaderProgram.h"

#ifndef NDEBUG
#define GLErrorCheck(x) ClearOpenGLError();\
							x;\
							if (CheckOpenGLError(#x, __FILE__, __LINE__)) __debugbreak();
#else
#define GLErrorCheck(x) x
#endif


#define numVBOs 4

namespace CC {
	class Renderer {
	private:
		Window& window;

		// a bogus vao that is never actually used. do not remove
		GLuint vao;
		GLuint vbo[numVBOs];
		
	public:
		Renderer(Window& window);
		void SetClearColor(float r, float g, float b, float a);
		void ClearScreen();
		void SetActiveTexture(const unsigned int& texture);
		void Draw(const Model& model, const unsigned int& vao, const ShaderProgram& shader);
		void SwapBuffers();
		unsigned int LoadTexture(const char* textureImagePath) const;
		Model LoadModel(const char* modelPath) const;
	};

	static void ClearOpenGLError() {
		int glErr;
		while (glErr = glGetError());
	}

	static bool CheckOpenGLError(const char* function, const char* file, int line) {
		bool foundError = false;
		int glErr;
		while (glErr = glGetError()) {
			std::cout << "[OpenGL Error] (" << glErr << "): " << function << " " << file << ":" << line << std::endl;
			foundError = true;
		}
		return foundError;
	}
}
#endif // RENDERER_H