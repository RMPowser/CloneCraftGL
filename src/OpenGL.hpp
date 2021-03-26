#pragma once

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
	#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>



#include <iostream>

#ifndef NDEBUG
	#define ASSERT(x) if (!(x)) __debugbreak();
	#define GLErrorCheck(x) ClearOpenGLError();\
							x;\
							if (CheckOpenGLError(#x, __FILE__, __LINE__)) __debugbreak();
#else
	#define ASSERT(x) x
	#define GLErrorCheck(x) x
#endif


inline static void ClearOpenGLError() {
	int glErr;
	while (glErr = glGetError());
}

inline static bool CheckOpenGLError(const char* function, const char* file, int line) {
	bool foundError = false;
	int glErr;
	while (glErr = glGetError()) {
		std::cout << "[OpenGL Error] (" << glErr << "): " << function << " " << file << ":" << line << std::endl;
		foundError = true;
	}
	return foundError;
}