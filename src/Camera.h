#pragma once

#include "Math.hpp"
#include "Window.h"

class Camera {
private:
	float fovY = 90;

	void MakeViewMatrix();

public:
	Mat4 vMat;
	Mat4 pMat;
	Vec3 position;
	Vec3 rotation; // for storing angles about each axis

	Camera(const Vec3& spawnLocation, Window& window);

	Vec4 GetForwardAxis();
	Vec4 GetRightAxis();
	Vec4 GetUpAxis();
	void Update(float aspectRatio);
	inline const float GetFOV() const { return fovY; }
	void SetFOV(float fovY, float aspectRatio);
	void RecreateProjectionMatrix(float aspectRatio);

	bool operator==(const Camera& other) const;
	bool operator!=(const Camera& other) const;
};
