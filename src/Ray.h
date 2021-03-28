#pragma once

#include "Math.hpp"

class Ray {
private:
	Vec3 rayStart;
	Vec3 rayEnd;
	Vec3 direction;

public:
	Ray(const Vec3& position, const Vec3& direction);

	void Step(float scale);
	
	inline float GetLength() { return sqrtf(powf((rayStart.x - rayEnd.x), 2) + powf((rayStart.y - rayEnd.y), 2) + powf((rayStart.z - rayEnd.z), 2)); }
	inline const Vec3& GetEnd() const { return rayEnd; }
};