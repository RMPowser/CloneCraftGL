#include "Camera.h"

namespace CC {
	void Camera::MakeViewMatrix() {
		Mat4 v = IdentityMatrix();
		v *= MakeRotateXMatrix(rotation[0] * RADIAN);
		v *= MakeRotateYMatrix(rotation[1] * RADIAN);
		v *= MakeRotateZMatrix(rotation[2] * RADIAN);
		v *= MakeTranslationMatrix({ -position[0], -position[1], -position[2], -position[3] });

		vMat = v;
	}
	
	Camera::Camera() {
	}

	Vec4 Camera::GetForwardAxis() {
		Vec4 r { 0, 0, 1, 1 }; // -z axis is forward
		Mat4 rotateY = MakeRotateYMatrix(-rotation[1] * RADIAN);

		r = rotateY * r;
		return r;
	}
	
	Vec4 Camera::GetRightAxis() {
		Vec4 r = GetForwardAxis();
		r = Cross(r, Vec4(0, -1, 0, 1));
		return r;
	}

	Vec4 Camera::GetUpAxis() {
		Vec4 forwardAxis = GetForwardAxis();
		Vec4 rightAxis = GetRightAxis();
		return Cross(forwardAxis, rightAxis);
	}

	void Camera::Update() {
		MakeViewMatrix();
	}

	bool Camera::operator==(const Camera& other) const {
		return (pMat == other.pMat &&
				vMat == other.vMat &&
				rotation == other.rotation &&
				position == other.position);
	}

	bool Camera::operator!=(const Camera& other) const {
		return !(*this == other);
	}
}