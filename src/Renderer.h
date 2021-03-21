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

#define numVAOs 1
#define numVBOs 4

namespace CC {
	class Renderer {
	private:
		Window window;
		Camera cam;

		Mat4 mvMat, invTrns_mvMat;
		Model model;
		Light light;
		Vec4 globalAmbient;

		GLuint renderingProgram;
		GLuint vao[numVAOs];
		GLuint vbo[numVBOs];
		GLuint textureAtlas;

		// locations for shader uniform variables
		GLuint L_GlobalAmbLoc, L_AmbLoc, L_DiffLoc, L_SpecLoc, L_PosLoc, M_AmbLoc, M_DiffLoc, M_SpecLoc, M_ShiLoc, mvMatLoc, pMatLoc, nMatLoc;

		void CreatePMat();

		// utilities
		GLuint CreateShaderProgram(const char* vShaderCode, const char* fShaderCode);
		std::string ReadShaderSource(const char* filePath);
		void PrintShaderLog(GLuint shader);
		void PrintProgramLog(int prog);
		bool CheckOpenGLError();
		void SetupVertices();
		void SetupLightData(Mat4 vMatrix);
		GLuint LoadTexture(const char* textureImagePath);
		Model LoadModel(const char* modelFilePath);

	public:

		Renderer();
		void Update(double dt);
		void Draw(double dt);
		inline Window& GetWindow() { return window; }
	};
}

#endif // RENDERER_H