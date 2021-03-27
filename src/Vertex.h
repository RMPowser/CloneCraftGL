#pragma once

#include "Math.hpp"

struct Vertex {
	Vec3 position = { 0, 0, 0 }; // 12 bytes

	bool operator==(const Vertex& other) const {
		return (position == other.position);
	}

	bool operator!=(const Vertex& other) const {
		return !(*this == other);
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& v) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, v.position.x);
			boost::hash_combine(seed, v.position.y);
			boost::hash_combine(seed, v.position.z);
			return seed;
		}
	};
}