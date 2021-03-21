#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"

namespace CC {
	struct Light {
		Vec4 ambient;
		Vec4 diffuse;
		Vec4 specular;
		Vec3 position;
	};
}
#endif // !LIGHT_H
