#ifndef VECTOR_H
#define VECTOR_H

#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace CC {
	class Vec2 {
	private:
		glm::vec2 glmVec;

	public:
		Vec2();
		Vec2(float x, float y);
		Vec2(const Vec2& other);

		float& operator[](unsigned int i);
		float const& operator[](unsigned int i) const;
		Vec2& operator=(const Vec2& other);
		Vec2& operator+=(const Vec2& other);
		Vec2& operator-=(const Vec2& other);
		Vec2& operator*=(const float& f);
		Vec2 operator+(const Vec2& other) const;
		Vec2 operator-(const Vec2& other) const;
		Vec2 operator*(const float f) const;
		bool operator==(const Vec2& other) const;
		bool operator!=(const Vec2& other) const;
	};

	class Vec4 {
		friend class Mat4;
	private:
		glm::vec4 glmVec;

	public:
		Vec4();
		Vec4(float x, float y, float z, float w);
		Vec4(const Vec4& other);
		
		float& operator[](unsigned int i);
		float const& operator[](unsigned int i) const;
		Vec4& operator=(const Vec4& other);
		Vec4& operator+=(const Vec4& other);
		Vec4& operator-=(const Vec4& other);
		Vec4& operator*=(const float& f);
		Vec4 operator+(const Vec4& other) const;
		Vec4 operator-(const Vec4& other) const;
		Vec4 operator*(const float f) const;
		bool operator==(const Vec4& other) const;
		bool operator!=(const Vec4& other) const;
	};
}

#endif // VECTOR_H