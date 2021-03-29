#pragma once

#include "CubeMap.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Renderer.h"


class SkyBox {
private:
	CubeMap cubemap;
	ShaderProgram shader;
	Vec3 skyboxVertices[36] = {
		// positions          
		{-1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		 {1.0f, -1.0f, -1.0f},
		 {1.0f, -1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},

		{-1.0f, -1.0f,  1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},

		 {1.0f, -1.0f, -1.0f},
		 {1.0f, -1.0f,  1.0f},
		 {1.0f,  1.0f,  1.0f},
		 {1.0f,  1.0f,  1.0f},
		 {1.0f,  1.0f, -1.0f},
		 {1.0f, -1.0f, -1.0f},

		{-1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		 {1.0f,  1.0f,  1.0f},
		 {1.0f,  1.0f,  1.0f},
		 {1.0f, -1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},

		{-1.0f,  1.0f, -1.0f},
		 {1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{-1.0f,  1.0f, -1.0f},

		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f, -1.0f},
		 {1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		 {1.0f, -1.0f,  1.0f}
	};

public:
	SkyBox();
	~SkyBox();

	void Load(std::vector<std::string>& cubeMapFilePaths);
	void Draw(const Renderer& renderer, const Camera& cam);
};