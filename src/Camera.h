#pragma once

#include "Math.hpp"
#include "Window.h"
#include "Plane.h"

class Camera {
private:
	float fovY = 90;
	float zNear = 0.01f;
	float zFar = 1000.f;
	float aspectRatio;
	
	// frustum stuff
	enum class Planes {
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far
	};
	Plane frustum[6];


	void MakeViewMatrix();
	void UpdateFrustum();

public:
	Mat4 vMat;
	Mat4 pMat;
	Vec3 position;
	Vec3 rotation; // for storing angles about each axis

	Camera(const Vec3& spawnLocation, Window& window);
	Camera(const Vec3& spawnLocation, const Vec3& rotation, Window& window);

	Vec4 GetForwardAxis();
	Vec4 GetRightAxis();
	Vec4 GetUpAxis();
	void Update(float aspectRatio);
	inline const float GetFOV() const { return fovY; }
	void SetFOV(float fovY, float aspectRatio);
	void RecreateProjectionMatrix(float aspectRatio);
	bool IsPointInFrustum(const Vec3& point);
	bool IsBoxInFrustum(const Vec3& min, const Vec3& max);

	void PrintDebugInfo();

	bool operator==(const Camera& other) const;
	bool operator!=(const Camera& other) const;
};
