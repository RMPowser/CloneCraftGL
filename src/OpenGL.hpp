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

#include "GLErrorUtils.h"