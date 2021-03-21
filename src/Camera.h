#ifndef CAMERA_H
#define CAMERA_H

#include "Math.hpp"

namespace CC {
	class Camera {
		Mat4 vMat;
		Mat4 pMat;
		Vec4 position;
		Vec4 rotation; // for storing angles about each axis

		void MakeViewMatrix();

	public:
		Camera();

		Vec4 GetForwardAxis();
		Vec4 GetRightAxis();
		Vec4 GetUpAxis();
		void Update();

		inline Mat4& VMat() { return vMat; }
		inline Mat4& PMat() { return pMat; }
		inline Vec4& Position() { return position; }
		inline Vec4& Rotation() { return rotation; }

		bool operator==(const Camera& other) const;
		bool operator!=(const Camera& other) const;
	};
}
#endif // !CAMERA_H