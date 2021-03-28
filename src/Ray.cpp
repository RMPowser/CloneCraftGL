#include "Ray.h"

Ray::Ray(const Vec3& position, const Vec3& direction)
	: rayStart(position), rayEnd(position), direction(direction) {

}

void Ray::Step(float scale) {
	float yaw = RADIAN * (direction.y + 90);
	float pitch = RADIAN * (direction.x);

	rayEnd.x -= cos(yaw) * scale;
	rayEnd.z -= sin(yaw) * scale;
	rayEnd.y -= tan(pitch) * scale;
}
