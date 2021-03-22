#ifndef MATH_HPP
#define MATH_HPP

#include "Vector.h"
#include "Matrix.h"
#include <cmath>

#define PI 3.14159265358979f
#define RADIAN (PI / 180.0f)

namespace CC {

	static inline Mat4 MakeProjectionMatrix(float fovY, float aspect, float zNear, float zFar) {
		glm::mat4 g = glm::perspective(fovY, aspect, zNear, zFar);
		Mat4 r;
		for (size_t i = 0; i < 16; i++) {
			r[i] = g[i % 4][i / 4];
		}

		return r;
	}

	static inline Mat4 MakeTranslationMatrix(const Vec4& p) {
		return Mat4( 1.0, 0.0, 0.0, p[0],
					 0.0, 1.0, 0.0, p[1],
					 0.0, 0.0, 1.0, p[2],
					 0.0, 0.0, 0.0, 1.0);
	}

	static inline Mat4 MakeRotateXMatrix(float rad) {
		return Mat4(1.0,      0.0,       0.0, 0.0,
					0.0, cos(rad), -sin(rad), 0.0,
					0.0, sin(rad),  cos(rad), 0.0,
					0.0,      0.0,       0.0, 1.0);
	}

	static inline Mat4 MakeRotateYMatrix(float rad) {
		return Mat4( cos(rad), 0.0, sin(rad), 0.0,
					     0.0,  1.0,      0.0, 0.0,
					-sin(rad), 0.0, cos(rad), 0.0,
					      0.0, 0.0,      0.0, 1.0);
	}

	static inline Mat4 MakeRotateZMatrix(float rad) {
		return Mat4(cos(rad), -sin(rad), 0.0, 0.0,
					sin(rad),  cos(rad), 0.0, 0.0,
					     0.0,       0.0, 1.0, 0.0,
					     0.0,       0.0, 0.0, 1.0);
	}

	static inline Mat4 MakeScaleMatrix(float x, float y, float z) {
		return Mat4(  x, 0.0, 0.0, 0.0,
					0.0,   y, 0.0, 0.0,
					0.0, 0.0,   z, 0.0,
					0.0, 0.0, 0.0, 1.0);
	}

	static inline Vec4 Normalize(const Vec4& v) {
		if (v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0) {
			return Vec4(0, 0, 0, 0);
		}
		glm::vec4 g = glm::normalize(glm::vec4(v[0], v[1], v[2], v[3]));
		Vec4 r;
		for (size_t i = 0; i < 4; i++) {
			r[i] = g[i];
		}

		return r;
	}

	static inline Mat4 Transpose(const Mat4& m) {
		return Mat4 {
			m[0], m[4], m[8], m[12],
			m[1], m[5], m[9], m[13],
			m[2], m[6], m[10], m[14],
			m[3], m[7], m[11], m[15],
		};
	}

	static inline Mat4 Inverse(const Mat4& m) {
		glm::mat4 g;
		for (size_t i = 0; i < 16; i++) {
			g[i % 4][i / 4] = m[i];
		}

		g = glm::transpose(glm::inverse(g));

		Mat4 r;
		for (size_t i = 0; i < 16; i++) {
			r[i] = g[i % 4][i / 4];
		}

		return r;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="p">camera position</param>
	/// <param name="t">target position</param>
	/// <returns></returns>
	static inline Mat4 MakeLookAtMatrix(const Vec4& p, const Vec4& t) {
		glm::mat4 g = glm::lookAt(glm::vec3(p[0], p[1], p[2]), glm::vec3(t[0], t[1], t[2]), glm::vec3(0.0f, 1.0f, 0.0f));
		Mat4 r;
		for (size_t i = 0; i < 16; i++) {
			r[i] = g[i % 4][i / 4];
		}

		return r;
	}

	static inline Mat4 IdentityMatrix() {
		return { 1, 0, 0, 0,
				 0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 };
	}

	static inline Vec4 Cross(Vec4& v1, Vec4& v2) {
		glm::vec3 g = glm::cross(glm::vec3(v1[0], v1[1], v1[2]), glm::vec3(v2[0], v2[1], v2[2]));
		return Vec4(g[0], g[1], g[2], 0.0);
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
}

#endif // MATH_HPP