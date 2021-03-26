#pragma once

#ifndef _WIN32
	#define _WIN32
#endif

#define GATEWARE_ENABLE_AUDIO
#define GATEWARE_ENABLE_CORE
#define GATEWARE_ENABLE_SYSTEM
#define GATEWARE_DISABLE_GLOG
#define GATEWARE_DISABLE_GRAPHICS
#define GATEWARE_DISABLE_GVULKANSURFACE
#define GATEWARE_DISABLE_MATH
#define GATEWARE_DISABLE_MATH2D
#define GATEWARE_ENABLE_INPUT
#include "Gateware/Gateware.h"