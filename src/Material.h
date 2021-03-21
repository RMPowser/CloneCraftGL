#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector.h"

namespace CC {
	struct Material {
		Vec4 ambient;
		Vec4 diffuse;
		Vec4 specular;
		float shininess;
	};

	namespace Materials {
		static Material gold = {
			Vec4{ 0.2473f, 0.1995f, 0.0745f, 1 },
			Vec4{ 0.7516f, 0.6065f, 0.2265f, 1 },
			Vec4{ 0.6283f, 0.5558f, 0.3661f, 1 },
			51.2f
		};

		static Material silver = {
			Vec4{ 0.1923f, 0.1923f, 0.1923f, 1 },
			Vec4{ 0.5075f, 0.5075f, 0.5075f, 1 },
			Vec4{ 0.5083f, 0.5083f, 0.5083f, 1 },
			51.2f
		};

		static Material bronze = {
			Vec4{ 0.2125f, 0.1275f, 0.0540f, 1 },
			Vec4{ 0.7140f, 0.4284f, 0.1814f, 1 },
			Vec4{ 0.3935f, 0.2719f, 0.1667f, 1 },
			25.6f
		};
	}
}
#endif // !MATERIAL_H
