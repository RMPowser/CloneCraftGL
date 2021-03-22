#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "Math.hpp"
#include "Material.h"
#include "Vertex.h"

using namespace std;

namespace CC {
	struct Model {
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		Material material;
	};
}

#endif // !MODEL_H
