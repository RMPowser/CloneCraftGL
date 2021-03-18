#include "Vector.h"

namespace CC {
	Vec2::Vec2()
		: glmVec(0.0f) {
	}

	Vec2::Vec2(float x, float y)
		: glmVec(x, y) {
	}

	Vec2::Vec2(const Vec2& other) {
		operator=(other);
	}

	float& Vec2::operator[](unsigned int i) {
		assert(i < 2);
		return glmVec[i];
	}

	float const& Vec2::operator[](unsigned int i) const {
		assert(i < 2);
		return glmVec[i];
	}

	Vec2& Vec2::operator=(const Vec2& other) {
		glmVec = other.glmVec;
		return *this;
	}

	Vec2& Vec2::operator+=(const Vec2& other) {
		glmVec += other.glmVec;
		return *this;
	}

	Vec2& Vec2::operator-=(const Vec2& other) {
		glmVec -= other.glmVec;
		return *this;
	}

	Vec2& Vec2::operator*=(const float& f) {
		glmVec *= f;
		return *this;
	}

	Vec2 Vec2::operator+(const Vec2& other) const {
		return Vec2(*this) += other;
	}

	Vec2 Vec2::operator-(const Vec2& other) const {
		return Vec2(*this) -= other;
	}

	Vec2 Vec2::operator*(const float f) const {
		return Vec2(*this) *= f;
	}

	bool Vec2::operator==(const Vec2& other) const {
		return glmVec == other.glmVec;
	}

	bool Vec2::operator!=(const Vec2& other) const {
		return !(*this == other);
	}


	Vec4::Vec4() 
		: glmVec(0.0f) {
	}

	Vec4::Vec4(float x, float y, float z, float w)
		: glmVec(x, y, z, w) {
	}

	Vec4::Vec4(const Vec4& other) {
		operator=(other);
	}

	float& Vec4::operator[](unsigned int i) {
		assert(i < 4);
		return glmVec[i];
	}

	float const& Vec4::operator[](unsigned int i) const {
		assert(i < 4);
		return glmVec[i];
	}

	Vec4& Vec4::operator=(const Vec4& other) {
		glmVec = other.glmVec;
		return *this;
	}

	Vec4& Vec4::operator+=(const Vec4& other) {
		glmVec += other.glmVec;
		return *this;
	}

	Vec4& Vec4::operator-=(const Vec4& other) {
		glmVec -= other.glmVec;
		return *this;
	}

	Vec4& Vec4::operator*=(const float& f) {
		glmVec *= f;
		return *this;
	}

	Vec4 Vec4::operator+(const Vec4& other) const {
		return Vec4(*this) += other;
	}

	Vec4 Vec4::operator-(const Vec4& other) const {
		return Vec4(*this) -= other;
	}

	Vec4 Vec4::operator*(const float f) const {
		return Vec4(*this) *= f;
	}

	bool Vec4::operator==(const Vec4& other) const {
		return glmVec == other.glmVec;
	}

	bool Vec4::operator!=(const Vec4& other) const {
		return !(*this == other);
	}
}