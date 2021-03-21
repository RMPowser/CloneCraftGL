#ifndef VERTEX_H
#define VERTEX_H

#include "Math.hpp"

namespace CC {
	struct Vertex {
		Vec3 position = { 0, 0, 0 }; // 12 bytes
		Vec3 normal = { 0, 0, 0 }; // 12 bytes
		Vec2 texCoord = { 0, 0 }; // 8 bytes

		bool operator==(const Vertex& other) const {
			return (position == other.position &&
					texCoord == other.texCoord);
		}

		bool operator!=(const Vertex& other) const {
			return !(*this == other);
		}
	};
}

#endif // VERTEX_H