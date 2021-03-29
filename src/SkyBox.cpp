#include "SkyBox.h"

#include "OpenGL.hpp"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexBufferArray.h"

SkyBox::SkyBox() {
	shader.Load("res/shaders/skybox.shader");
}

SkyBox::~SkyBox() {

}

void SkyBox::Load(std::vector<std::string>& cubeMapFilePaths) {
	cubemap.Load(cubeMapFilePaths);
}

void SkyBox::Draw(const Renderer& renderer, const Camera& cam) {
	GLErrorCheck(glDepthMask(GL_FALSE));

	Mat4 vMatWithoutTranslation = cam.vMat;
	vMatWithoutTranslation.Position() = { 0, 0, 0, 1 };
	
	VertexBufferArray va;
	VertexBuffer vb(skyboxVertices, sizeof(skyboxVertices));
	VertexBufferLayout layout;
	layout.Push<float>(3, false, 0);

	va.AddBuffer(vb, layout);

	shader.Bind();
	cubemap.Bind(0);
	shader.SetUniform1i("skybox", 0);
	shader.SetUniformMatrix4fv("projection", cam.pMat);
	shader.SetUniformMatrix4fv("view", vMatWithoutTranslation);


	renderer.DrawArrays(va, 36, shader);

	glDepthMask(GL_TRUE);
}
