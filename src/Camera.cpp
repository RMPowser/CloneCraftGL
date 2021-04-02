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
	Mat4 vp = Transpose(pMat * vMat);

	Plane& l(frustum[(int)Planes::Left]);
	Plane& r(frustum[(int)Planes::Right]);
	Plane& b(frustum[(int)Planes::Bottom]);
	Plane& t(frustum[(int)Planes::Top]);
	Plane& n(frustum[(int)Planes::Near]);
	Plane& f(frustum[(int)Planes::Far]);

	l.A = vp[3][0] + vp[0][0];
	l.B = vp[3][1] + vp[0][1];
	l.C = vp[3][2] + vp[0][2];
	l.D = vp[3][3] + vp[0][3];

	r.A = vp[3][0] - vp[0][0];
	r.B = vp[3][1] - vp[0][1];
	r.C = vp[3][2] - vp[0][2];
	r.D = vp[3][3] - vp[0][3];

	b.A = vp[3][0] + vp[1][0];
	b.B = vp[3][1] + vp[1][1];
	b.C = vp[3][2] + vp[1][2];
	b.D = vp[3][3] + vp[1][3];

	t.A = vp[3][0] - vp[1][0];
	t.B = vp[3][1] - vp[1][1];
	t.C = vp[3][2] - vp[1][2];
	t.D = vp[3][3] - vp[1][3];

	n.A = vp[3][0] + vp[2][0];
	n.B = vp[3][1] + vp[2][1];
	n.C = vp[3][2] + vp[2][2];
	n.D = vp[3][3] + vp[2][3];

	f.A = vp[3][0] - vp[2][0];
	f.B = vp[3][1] - vp[2][1];
	f.C = vp[3][2] - vp[2][2];
	f.D = vp[3][3] - vp[2][3];

// 	float normalLength;
// 	Vec3 normalized;
// 	for (auto& plane : frustum) {
// 		normalLength = sqrt((plane.A * plane.A) + (plane.B * plane.B) + (plane.C * plane.C));
// 		plane.A /= normalLength;
// 		plane.B /= normalLength;
// 		plane.C /= normalLength;
// 		plane.D /= normalLength;
// 	}
}

Camera::Camera(const Vec3& spawnLocation, Window& window) {
	position = spawnLocation;
	glfwSetWindowSizeCallback(window.glfwWindow, framebuffer_size_callback);
}

Camera::Camera(const Vec3& spawnLocation, const Vec3& _rotation, Window& window) {
	position = spawnLocation;
	rotation = _rotation;
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

bool Camera::IsPointInFrustum(const Vec3& p) {
	// check box outside/inside of frustum
	for (auto& i : frustum) {
		int out = 0;
		Vec4 plane(i.A, i.B, i.C, i.D);
		out += ((Dot(plane, Vec4(p, 1.0f)) < 0.0) ? 1 : 0);
		if (out == 1) return false;
	}

	return true;
}

bool Camera::IsBoxInFrustum(const Vec3& min, const Vec3& max) {
	// check box outside/inside of frustum
	for (auto& i : frustum) {
		int out = 0;
		Vec4 plane(i.A, i.B, i.C, i.D);
		out += ((Dot(plane, Vec4(min.x, min.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(max.x, min.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(min.x, max.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(max.x, max.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(min.x, min.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(max.x, min.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(min.x, max.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
		out += ((Dot(plane, Vec4(max.x, max.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
		if (out == 8) return false;
	}

	return true;
}

void Camera::PrintDebugInfo() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { 0, 1 };
	SetConsoleCursorPosition(hStdout, position);

	Plane& l(frustum[(int)Planes::Left]);
	Plane& r(frustum[(int)Planes::Right]);
	Plane& b(frustum[(int)Planes::Bottom]);
	Plane& t(frustum[(int)Planes::Top]);
	Plane& n(frustum[(int)Planes::Near]);
	Plane& f(frustum[(int)Planes::Far]);

	printf(
		R"(Left Plane -------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

Right Plane ------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

Bottom Plane ------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

Top Plane ------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

Near Plane ------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

Far Plane ------------------------------------------
A: %f	B: %f	C: %f	D: %f                         

)",
l.A, l.B, l.C, l.D,
r.A, r.B, r.C, r.D,
b.A, b.B, b.C, b.D,
t.A, t.B, t.C, t.D,
n.A, n.B, n.C, n.D,
f.A, f.B, f.C, f.D);
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
