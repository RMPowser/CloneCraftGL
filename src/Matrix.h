#pragma once

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "Vector.h"

class Mat4 {
private:
	union {
		struct {
			Vec4 col0;
			Vec4 col1;
			Vec4 col2;
			Vec4 col3;
		};
		glm::mat4 glmMat;
	};

public:
	Mat4();
	Mat4(const Mat4& other);
	Mat4(float x0, float y0, float z0, float w0,
		 float x1, float y1, float z1, float w1,
		 float x2, float y2, float z2, float w2,
		 float x3, float y3, float z3, float w3);

	float* data() const;
	Vec4& SideAxis();
	Vec4& UpAxis();
	Vec4& FwdAxis();
	Vec4& Position();
	const Vec4& SideAxis() const;
	const Vec4& UpAxis() const;
	const Vec4& FwdAxis() const;
	const Vec4& Position() const;


	Vec4& operator[](unsigned int i);
	const Vec4& operator[](unsigned int i) const;
	Mat4& operator=(const Mat4& other);
	Mat4& operator+=(const Mat4& other);
	Mat4& operator-=(const Mat4& other);
	Mat4& operator*=(const Mat4& other);
	Mat4& operator*=(const float& f);
	const Mat4 operator+(const Mat4 other) const;
	const Mat4 operator-(const Mat4 other) const;
	const Mat4 operator*(const Mat4 other) const;
	const Vec4 operator*(const Vec4 other) const;
	bool operator==(const Mat4 other) const;
	bool operator!=(const Mat4 other) const;
};