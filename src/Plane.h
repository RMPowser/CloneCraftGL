#pragma once

#include "Math.hpp"

class Plane {
private:


public:
	Vec3 normal; // A, B, C
	float distanceToOrigin; // D

	bool operator==(const Plane& other) const {
		return (normal == other.normal &&
				distanceToOrigin == other.distanceToOrigin);
	}

	bool operator!=(const Plane& other) const {
		return !(*this == other);
	}
};