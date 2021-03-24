#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Math.hpp"
#include "Material.h"
#include "Vertex.h"


namespace CC {
	struct Model {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Material material;
	};
}

#endif // !MODEL_H
