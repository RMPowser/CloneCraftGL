#ifndef VECTOR_H
#define VECTOR_H

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <boost/container_hash/hash.hpp>

namespace CC {
	class Vec2 {
	public:
		union {
			struct {
				float x, z;
			};
			glm::vec2 glmVec;
		};
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

	class Vec4;

	class Vec3 {
	public:
		union {
			struct {
				float x, y, z;
			};
			glm::vec3 glmVec;
		};
		Vec3();
		Vec3(float x, float y, float z);
		Vec3(const Vec4& xyzw);
		Vec3(const Vec3& other);

		float& operator[](unsigned int i);
		float const& operator[](unsigned int i) const;
		Vec3& operator=(const Vec3& other);
		Vec3& operator+=(const Vec3& other);
		Vec3& operator-=(const Vec3& other);
		Vec3& operator*=(const float& f);
		Vec3 operator+(const Vec3& other) const;
		Vec3 operator-(const Vec3& other) const;
		Vec3 operator*(const float f) const;
		bool operator==(const Vec3& other) const;
		bool operator!=(const Vec3& other) const;
	};

	class Vec4 {
	public:
		union {
			struct {
				float x, y, z, w;
			};
			glm::vec4 glmVec;
		};

		Vec4();
		Vec4(float x, float y, float z, float w);
		Vec4(const Vec3& xyz, float w);
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

namespace std {
	template<> struct hash<CC::Vec2> {
		size_t operator()(CC::Vec2 const& v) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, v.x);
			boost::hash_combine(seed, v.z);
			return seed;
		}
	};
}


#endif // VECTOR_H