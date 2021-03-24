#ifndef RENDERER_H
#define RENDERER_H

#include "Math.hpp"
#include "Gateware.hpp"
#include "Camera.h"
#include "Window.h"
#include "Model.h"
#include "Light.h"

#include <string>
#include <iostream>
#include <fstream>

#define numVBOs 4

namespace CC {
	class Renderer {
	private:
		Window& window;

		Mat4 mvMat, nMat;
		PositionalLight light;
		Vec4 globalAmbient;

		// a bogus vao that is never actually used. do not remove
		GLuint vao;
		GLuint vbo[numVBOs];
		

		// locations for shader uniform variables
		GLuint L_GlobalAmbLoc, L_AmbLoc, L_DiffLoc, L_SpecLoc, L_PosLoc, M_AmbLoc, M_DiffLoc, M_SpecLoc, M_ShiLoc, mvMatLoc, pMatLoc, nMatLoc;

		// utilities
		GLuint CreateShaderProgram(const char* vShaderCode, const char* fShaderCode);
		std::string ReadShaderSource(const char* filePath);
		void PrintShaderLog(GLuint shader);
		void PrintProgramLog(int prog);
		bool CheckOpenGLError() const;

		void SetupLightData(Mat4& vMatrix, PositionalLight& light, Model& model);


	public:
		GLuint renderingProgram;

		Renderer(Window& window);
		void SetClearColor(float r, float g, float b, float a);
		void ClearScreen();
		void SetActiveTexture(const unsigned int& texture);
		void Draw(Camera& camera, const Model& model, const Mat4& mMat, const unsigned int& vao);
		void SwapBuffers();
		unsigned int LoadTexture(const char* textureImagePath) const;
		Model LoadModel(const char* modelPath) const;
	};
}

#endif // RENDERER_H