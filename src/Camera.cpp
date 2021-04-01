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

	UpdateFrustum();
}
	
void Camera::UpdateFrustum() {
	Mat4 vp = pMat * vMat;

	frustum[(int)Planes::Left].normal = { vp[3][0] + vp[0][0],
										  vp[3][1] + vp[0][1],
										  vp[3][2] + vp[0][2] };
	frustum[(int)Planes::Left].distanceToOrigin = vp[3][3] + vp[0][3];

	frustum[(int)Planes::Right].normal = { vp[3][0] - vp[0][0],
										   vp[3][1] - vp[0][1],
										   vp[3][2] - vp[0][2] };
	frustum[(int)Planes::Right].distanceToOrigin = vp[3][3] - vp[0][3];

	frustum[(int)Planes::Near].normal = { vp[3][0] + vp[2][0],
										   vp[3][1] + vp[2][1],
										   vp[3][2] + vp[2][2] };
	frustum[(int)Planes::Near].distanceToOrigin = vp[3][3] + vp[2][3];
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

void Camera::RecreateProjectionMatrix(float _aspectRatio) {
	aspectRatio = _aspectRatio;

	pMat = MakeProjectionMatrix(fovY * RADIAN, aspectRatio, zNear, zFar);

	UpdateFrustum();
}

bool Camera::IsPointInFrustum(const Vec3& point) {
	float dL = frustum[(int)Planes::Left].normal.x * point.x + frustum[(int)Planes::Left].normal.y * point.y + frustum[(int)Planes::Left].normal.z * point.z + frustum[(int)Planes::Left].distanceToOrigin;
	float dR = frustum[(int)Planes::Right].normal.x * point.x + frustum[(int)Planes::Right].normal.y * point.y + frustum[(int)Planes::Right].normal.z * point.z + frustum[(int)Planes::Right].distanceToOrigin;
	float dN = frustum[(int)Planes::Near].normal.x * point.x + frustum[(int)Planes::Near].normal.y * point.y + frustum[(int)Planes::Near].normal.z * point.z + frustum[(int)Planes::Near].distanceToOrigin;

	if (dL > 0 && dR > 0 && dN > 0) {
		return true;
	}
	else {
		return false;
	}
}

void Camera::PrintDebugInfo() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { 0, 1 };
	SetConsoleCursorPosition(hStdout, position);

	printf(
		R"(Left Plane -------------------------------------------
A: %f                                                 
B: %f                                                
C: %f                                                
D: %f                                                

Right Plane ------------------------------------------
A: %f                                                 
B: %f                                                
C: %f                                                
D: %f                                                

Near Plane ------------------------------------------
A: %f                                                 
B: %f                                                
C: %f                                                
D: %f                                                

)", frustum[(int)Planes::Left].normal.x,
frustum[(int)Planes::Left].normal.y,
frustum[(int)Planes::Left].normal.z,
frustum[(int)Planes::Left].distanceToOrigin,
frustum[(int)Planes::Right].normal.x,
frustum[(int)Planes::Right].normal.y,
frustum[(int)Planes::Right].normal.z,
frustum[(int)Planes::Right].distanceToOrigin,
frustum[(int)Planes::Near].normal.x,
frustum[(int)Planes::Near].normal.y,
frustum[(int)Planes::Near].normal.z,
frustum[(int)Planes::Near].distanceToOrigin);
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
