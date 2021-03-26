#include "Camera.h"

bool resized = true;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	resized = true;
}


void Camera::MakeViewMatrix() {
	Mat4 v = IdentityMatrix();
	v *= MakeRotateXMatrix(rotation[0] * RADIAN);
	v *= MakeRotateYMatrix(rotation[1] * RADIAN);
	v *= MakeRotateZMatrix(rotation[2] * RADIAN);
	v *= MakeTranslationMatrix({ -position.x, -position.y, -position.z, 1 });

	vMat = v;
}
	
Camera::Camera(const Vec3& spawnLocation, Window& window) {
	position = spawnLocation;
	glfwSetWindowSizeCallback(window.glfwWindow, framebuffer_size_callback);
}

Vec4 Camera::GetForwardAxis() {
	Vec4 r { 0, 0, 1, 1 }; // -z axis is forward
	r = MakeRotateYMatrix(-rotation[1] * RADIAN) * r;
	return r;
}
	
Vec4 Camera::GetRightAxis() {
	Vec4 fwd { 0, 0, 1, 1 }; // -z axis is forward
	fwd = MakeRotateYMatrix(-rotation[1] * RADIAN) * fwd;

	return Cross(fwd, Vec4(0, -1, 0, 1));
}

Vec4 Camera::GetUpAxis() {
	Vec4 fwd { 0, 0, 1, 1 }; // -z axis is forward
	fwd = MakeRotateYMatrix(-rotation[1] * RADIAN) * fwd;

	Vec4 right = Cross(fwd, Vec4(0, -1, 0, 1));
		
	return Cross(fwd, right);
}

void Camera::Update(float aspectRatio) {
	MakeViewMatrix();
	if (resized) {
		RecreateProjectionMatrix(aspectRatio);
		resized = false;
	}
}

void Camera::SetFOV(float _fovY, float aspectRatio) {
	fovY = _fovY;
	RecreateProjectionMatrix(aspectRatio);
}

void Camera::RecreateProjectionMatrix(float aspectRatio) {
	pMat = MakeProjectionMatrix(fovY * RADIAN, aspectRatio, 0.01f, 1000.0f);
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
