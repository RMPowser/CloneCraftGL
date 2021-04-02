#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "OpenGL.hpp"
#include <cmath>

#define PI 3.14159265358979f
#define RADIAN (PI / 180.0f)


static inline Mat4 MakeProjectionMatrix(float fovY, float aspect, float zNear, float zFar) {
	Mat4 r;
	
	assert(abs(aspect) > 0);

	float tanHalfFovY = tan(fovY / 2.f);

	r[0][0] = 1 / (aspect * tanHalfFovY);
	r[1][1] = 1 / (tanHalfFovY);
	r[2][2] = -(zFar + zNear) / (zFar - zNear);
	r[2][3] = -1;
	r[3][2] = -(2 * zFar * zNear) / (zFar - zNear);
	r[3][3] = 0;

	return r;
}

static inline Mat4 MakeTranslationMatrix(const Vec4& p) {
	return Mat4(1.0, 0.0, 0.0, p[0],
				0.0, 1.0, 0.0, p[1],
				0.0, 0.0, 1.0, p[2],
				0.0, 0.0, 0.0, 1.0);
}

static inline Mat4 MakeRotateXMatrix(float rad) {
	return Mat4(1.0, 0.0, 0.0, 0.0,
				0.0, cos(rad), -sin(rad), 0.0,
				0.0, sin(rad), cos(rad), 0.0,
				0.0, 0.0, 0.0, 1.0);
}

static inline Mat4 MakeRotateYMatrix(float rad) {
	return Mat4(cos(rad), 0.0, sin(rad), 0.0,
				0.0, 1.0, 0.0, 0.0,
				-sin(rad), 0.0, cos(rad), 0.0,
				0.0, 0.0, 0.0, 1.0);
}

static inline Mat4 MakeRotateZMatrix(float rad) {
	return Mat4(cos(rad), -sin(rad), 0.0, 0.0,
				sin(rad), cos(rad), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0);
}

static inline Mat4 MakeScaleMatrix(float x, float y, float z) {
	return Mat4(x, 0.0, 0.0, 0.0,
				0.0, y, 0.0, 0.0,
				0.0, 0.0, z, 0.0,
				0.0, 0.0, 0.0, 1.0);
}

static inline float Dot(const Vec3& a, const Vec3& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

// w component is not ignored
static inline float Dot(const Vec4& a, const Vec4& b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

static inline Vec3 Normalize(const Vec3& v) {
	if (v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0) {
		return Vec3(0, 0, 0);
	}

	float length = sqrt(Dot(v, v));
	return Vec3(v.x / length, v.y / length, v.z / length);
}

// w component is ignored
static inline Vec4 Normalize(const Vec4& v) {
	if (v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0) {
		return Vec4(0, 0, 0, 0);
	}

	float length = sqrt(Dot(v, v));
	return Vec4(v.x / length, v.y / length, v.z / length, v.w);
}

static inline Mat4 Transpose(const Mat4& m) {
	return Mat4 {
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]
	};
}

static inline Mat4 Inverse(const Mat4& m) {
	Mat4 inv;
	float det;

	inv[0][0] = (m[1][1] * m[2][2] * m[3][3] -
				 m[1][1] * m[2][3] * m[3][2] -
				 m[2][1] * m[1][2] * m[3][3] +
				 m[2][1] * m[1][3] * m[3][2] +
				 m[3][1] * m[1][2] * m[2][3] -
				 m[3][1] * m[1][3] * m[2][2]);

	inv[1][0] = (-m[1][0] * m[2][2] * m[3][3] +
				 m[1][0] * m[2][3] * m[3][2] +
				 m[2][0] * m[1][2] * m[3][3] -
				 m[2][0] * m[1][3] * m[3][2] -
				 m[3][0] * m[1][2] * m[2][3] +
				 m[3][0] * m[1][3] * m[2][2]);

	inv[2][0] = (m[1][0] * m[2][1] * m[3][3] -
				 m[1][0] * m[2][3] * m[3][1] -
				 m[2][0] * m[1][1] * m[3][3] +
				 m[2][0] * m[1][3] * m[3][1] +
				 m[3][0] * m[1][1] * m[2][3] -
				 m[3][0] * m[1][3] * m[2][1]);

	inv[3][0] = (-m[1][0] * m[2][1] * m[3][2] +
				 m[1][0] * m[2][2] * m[3][1] +
				 m[2][0] * m[1][1] * m[3][2] -
				 m[2][0] * m[1][2] * m[3][1] -
				 m[3][0] * m[1][1] * m[2][2] +
				 m[3][0] * m[1][2] * m[2][1]);

	inv[0][1] = (-m[0][1] * m[2][2] * m[3][3] +
				 m[0][1] * m[2][3] * m[3][2] +
				 m[2][1] * m[0][2] * m[3][3] -
				 m[2][1] * m[0][3] * m[3][2] -
				 m[3][1] * m[0][2] * m[2][3] +
				 m[3][1] * m[0][3] * m[2][2]);

	inv[1][1] = (m[0][0] * m[2][2] * m[3][3] -
				 m[0][0] * m[2][3] * m[3][2] -
				 m[2][0] * m[0][2] * m[3][3] +
				 m[2][0] * m[0][3] * m[3][2] +
				 m[3][0] * m[0][2] * m[2][3] -
				 m[3][0] * m[0][3] * m[2][2]);

	inv[2][1] = (-m[0][0] * m[2][1] * m[3][3] +
				 m[0][0] * m[2][3] * m[3][1] +
				 m[2][0] * m[0][1] * m[3][3] -
				 m[2][0] * m[0][3] * m[3][1] -
				 m[3][0] * m[0][1] * m[2][3] +
				 m[3][0] * m[0][3] * m[2][1]);

	inv[3][1] = (m[0][0] * m[2][1] * m[3][2] -
				 m[0][0] * m[2][2] * m[3][1] -
				 m[2][0] * m[0][1] * m[3][2] +
				 m[2][0] * m[0][2] * m[3][1] +
				 m[3][0] * m[0][1] * m[2][2] -
				 m[3][0] * m[0][2] * m[2][1]);

	inv[0][2] = (m[0][1] * m[1][2] * m[3][3] -
				 m[0][1] * m[1][3] * m[3][2] -
				 m[1][1] * m[0][2] * m[3][3] +
				 m[1][1] * m[0][3] * m[3][2] +
				 m[3][1] * m[0][2] * m[1][3] -
				 m[3][1] * m[0][3] * m[1][2]);

	inv[1][2] = (-m[0][0] * m[1][2] * m[3][3] +
				 m[0][0] * m[1][3] * m[3][2] +
				 m[1][0] * m[0][2] * m[3][3] -
				 m[1][0] * m[0][3] * m[3][2] -
				 m[3][0] * m[0][2] * m[1][3] +
				 m[3][0] * m[0][3] * m[1][2]);

	inv[2][2] = (m[0][0] * m[1][1] * m[3][3] -
				 m[0][0] * m[1][3] * m[3][1] -
				 m[1][0] * m[0][1] * m[3][3] +
				 m[1][0] * m[0][3] * m[3][1] +
				 m[3][0] * m[0][1] * m[1][3] -
				 m[3][0] * m[0][3] * m[1][1]);

	inv[3][2] = (-m[0][0] * m[1][1] * m[3][2] +
				 m[0][0] * m[1][2] * m[3][1] +
				 m[1][0] * m[0][1] * m[3][2] -
				 m[1][0] * m[0][2] * m[3][1] -
				 m[3][0] * m[0][1] * m[1][2] +
				 m[3][0] * m[0][2] * m[1][1]);

	inv[0][3] = (-m[0][1] * m[1][2] * m[2][3] +
				 m[0][1] * m[1][3] * m[2][2] +
				 m[1][1] * m[0][2] * m[2][3] -
				 m[1][1] * m[0][3] * m[2][2] -
				 m[2][1] * m[0][2] * m[1][3] +
				 m[2][1] * m[0][3] * m[1][2]);

	inv[1][3] = (m[0][0] * m[1][2] * m[2][3] -
				 m[0][0] * m[1][3] * m[2][2] -
				 m[1][0] * m[0][2] * m[2][3] +
				 m[1][0] * m[0][3] * m[2][2] +
				 m[2][0] * m[0][2] * m[1][3] -
				 m[2][0] * m[0][3] * m[1][2]);

	inv[2][3] = (-m[0][0] * m[1][1] * m[2][3] +
				 m[0][0] * m[1][3] * m[2][1] +
				 m[1][0] * m[0][1] * m[2][3] -
				 m[1][0] * m[0][3] * m[2][1] -
				 m[2][0] * m[0][1] * m[1][3] +
				 m[2][0] * m[0][3] * m[1][1]);

	inv[3][3] = (m[0][0] * m[1][1] * m[2][2] -
				 m[0][0] * m[1][2] * m[2][1] -
				 m[1][0] * m[0][1] * m[2][2] +
				 m[1][0] * m[0][2] * m[2][1] +
				 m[2][0] * m[0][1] * m[1][2] -
				 m[2][0] * m[0][2] * m[1][1]);

	det = m[0][0] * inv[0][0] + m[0][1] * inv[1][0] + m[0][2] * inv[2][0] + m[0][3] * inv[3][0];

	if (det == 0)
		ASSERT(false);

	det = 1.0 / det;

	inv[0][0] = inv[0][0] * det;
	inv[0][1] = inv[0][1] * det;
	inv[0][2] = inv[0][2] * det;
	inv[0][3] = inv[0][3] * det;
	inv[1][0] = inv[1][0] * det;
	inv[1][1] = inv[1][1] * det;
	inv[1][2] = inv[1][2] * det;
	inv[1][3] = inv[1][3] * det;
	inv[2][0] = inv[2][0] * det;
	inv[2][1] = inv[2][1] * det;
	inv[2][2] = inv[2][2] * det;
	inv[2][3] = inv[2][3] * det;
	inv[3][0] = inv[3][0] * det;
	inv[3][1] = inv[3][1] * det;
	inv[3][2] = inv[3][2] * det;
	inv[3][3] = inv[3][3] * det;

	return inv;
}

// v will be modified
static inline void Negate(Vec3& v) {
	v = { -v.x, -v.y, -v.z };
}

// v will be modified
static inline void Negate(Vec4& v) {
	v = { -v.x, -v.y, -v.z, 1 };
}

static inline Vec3 Cross(const Vec3& x, const Vec3& y) {
	return Vec3(
		x.y * y.z - y.y * x.z,
		x.z * y.x - y.z * x.x,
		x.x * y.y - y.x * x.y);
}

static inline Vec4 Cross(const Vec4& x, const Vec4& y) {
	return Vec4(
		x.y * y.z - y.y * x.z,
		x.z * y.x - y.z * x.x,
		x.x * y.y - y.x * x.y,
		1);
}

/// <summary>
/// 
/// </summary>
/// <param name="p">camera position</param>
/// <param name="t">target position</param>
/// <returns></returns>
static inline Mat4 MakeLookAtMatrix(const Vec3& p, const Vec3& t) {
	Vec3 up = { 0, 1, 0 };
	
	Vec3 zAxis = Normalize(t - p);
	Vec3 xAxis = Normalize(Cross(zAxis, up));
	Vec3 yAxis = Cross(xAxis, zAxis);

	return { xAxis.x, xAxis.y, xAxis.z, -Dot(xAxis, p),
			xAxis.x, xAxis.y, xAxis.z, -Dot(yAxis, p),
			xAxis.x, xAxis.y, xAxis.z, -Dot(zAxis, p),
			0, 0, 0, 1 };
}

static inline Mat4 IdentityMatrix() {
	return { 1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1 };
}

static inline float clamp(float value, float lowerBound, float upperBound) {
	if (value < lowerBound) {
		return lowerBound;
	}
	else if (value > upperBound) {
		return upperBound;
	}
	else {
		return value;
	}
}