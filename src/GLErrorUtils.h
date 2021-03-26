#pragma once

#include "OpenGL.hpp"
#include <iostream>

#ifndef NDEBUG
	#define GLErrorCheck(x) ClearOpenGLError();\
							x;\
							if (CheckOpenGLError(#x, __FILE__, __LINE__)) __debugbreak();
#else
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