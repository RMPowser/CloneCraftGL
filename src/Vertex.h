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
					normal == other.normal &&
					texCoord == other.texCoord);
		}

		bool operator!=(const Vertex& other) const {
			return !(*this == other);
		}
	};
}

namespace std {
	template<> struct hash<CC::Vertex> {
		size_t operator()(CC::Vertex const& v) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, v.position.x);
			boost::hash_combine(seed, v.position.y);
			boost::hash_combine(seed, v.position.z);
			boost::hash_combine(seed, v.normal.x);
			boost::hash_combine(seed, v.normal.y);
			boost::hash_combine(seed, v.normal.z);
			boost::hash_combine(seed, v.texCoord.x);
			boost::hash_combine(seed, v.texCoord.z);
			return seed;
		}
	};
}

#endif // VERTEX_H