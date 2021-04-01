#include "Matrix.h"


Mat4::Mat4() {
	glmMat = { 1, 0, 0, 0,
			   0, 1, 0, 0,
			   0, 0, 1, 0,
			   0, 0, 0, 1 };
}

Mat4::Mat4(const Mat4& other) {
	operator=(other);
}

Mat4::Mat4(float x0, float x1, float x2, float x3,
		   float y0, float y1, float y2, float y3,
		   float z0, float z1, float z2, float z3,
		   float w0, float w1, float w2, float w3)
	: glmMat(x0, y0, z0, w0,
			 x1, y1, z1, w1,
			 x2, y2, z2, w2,
			 x3, y3, z3, w3) {
}

float* Mat4::data() const {
	return (float*)glm::value_ptr(glmMat);
}

Vec4& Mat4::SideAxis() {
	return col0;
}

Vec4& Mat4::UpAxis() {
	return col1;
}

Vec4& Mat4::FwdAxis() {
	return col2;
}

Vec4& Mat4::Position() {
	return col3;
}

const Vec4& Mat4::SideAxis() const {
	return col0;
}

const Vec4& Mat4::UpAxis() const {
	return col1;
}

const Vec4& Mat4::FwdAxis() const {
	return col2;
}

const Vec4& Mat4::Position() const {
	return col3;
}

Vec4& Mat4::operator[](unsigned int i) {
	assert(i < 4);

	switch (i) {
		case 0:
			return col0;
		case 1:
			return col1;
		case 2:
			return col2;
		case 3:
			return col3;
	}
}

const Vec4& Mat4::operator[](unsigned int i) const {
	assert(i < 4);

	switch (i) {
		case 0:
			return col0;
		case 1:
			return col1;
		case 2:
			return col2;
		case 3:
			return col3;
	}
}

Mat4& Mat4::operator=(const Mat4& other) {
	glmMat = other.glmMat;
	return *this;
}

Mat4& Mat4::operator+=(const Mat4& other) {
	glmMat += other.glmMat;
	return *this;
}

Mat4& Mat4::operator-=(const Mat4& other) {
	glmMat -= other.glmMat;
	return *this;
}

Mat4& Mat4::operator*=(const Mat4& other) {
	glmMat *= other.glmMat;
	return *this;
}

Mat4& Mat4::operator*=(const float& f) {
	glmMat *= f;
	return *this;
}

const Mat4 Mat4::operator+(const Mat4 other) const {
	return Mat4(*this) += other;
}

const Mat4 Mat4::operator-(const Mat4 other) const {
	return Mat4(*this) -= other;
}

const Mat4 Mat4::operator*(const Mat4 other) const {
	return Mat4(*this) *= other;
}

const Vec4 Mat4::operator*(const Vec4 other) const {
	glm::vec4 v = glmMat * other.glmVec;
	Vec4 r(v[0], v[1], v[2], v[3]);
	return r;
}

bool Mat4::operator==(const Mat4 other) const {
	return glmMat == other.glmMat;
}

bool Mat4::operator!=(const Mat4 other) const {
	return !(*this == other);
}