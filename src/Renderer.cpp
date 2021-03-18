#include "Renderer.h"
#include "OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define vShaderPath "./shaders/vShader.glsl"
#define fShaderPath "./shaders/fShader.glsl"
#define textureAtlasPath "./textures/textureAtlas.png"



float FOV = 60;
bool resized = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	resized = true;
}

namespace CC {
	void Renderer::CreatePMat() {
		aspect = window.GetAspectRatio();
		cam.PMat() = MakeProjectionMatrix(FOV * RADIAN, aspect, 0.01f, 1000.0f);
	}

	GLuint Renderer::CreateShaderProgram(const char* vShaderCode, const char* fShaderCode) {
		std::string vShaderStr = ReadShaderSource(vShaderCode);
		std::string fShaderStr = ReadShaderSource(fShaderCode);

		const char* vShaderSource = vShaderStr.c_str();
		const char* fShaderSource = fShaderStr.c_str();

		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vShader, 1, &vShaderSource, NULL);
		glShaderSource(fShader, 1, &fShaderSource, NULL);

		GLint vertCompiled;
		GLint fragCompiled;
		GLint linked;

		glCompileShader(vShader);
		CheckOpenGLError();
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
		if (vertCompiled != 1) {
			std::cout << "vertex compilation failed" << std::endl;
			PrintShaderLog(vShader);
		}

		glCompileShader(fShader);
		CheckOpenGLError();
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
		if (fragCompiled != 1) {
			std::cout << "fragment compilation failed" << std::endl;
			PrintShaderLog(fShader);
		}

		GLuint vfProgram = glCreateProgram();

		glAttachShader(vfProgram, vShader);
		glAttachShader(vfProgram, fShader);

		glLinkProgram(vfProgram);
		CheckOpenGLError();
		glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
		if (linked != 1) {
			std::cout << "linking failed" << std::endl;
			PrintProgramLog(vfProgram);
		}

		return vfProgram;
	}


	std::string Renderer::ReadShaderSource(const char* filePath) {
		using namespace std;

		string fileContents;
		ifstream fileStream(filePath, ios::in);
		string line = "";

		while (!fileStream.eof()) {
			getline(fileStream, line);
			fileContents.append(line + "\n");
		}

		fileStream.close();
		return fileContents;
	}


	void Renderer::PrintShaderLog(GLuint shader) {
		int len = 0;
		int chWrittn = 0;
		char* log;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			log = (char*)malloc(len);
			glGetShaderInfoLog(shader, len, &chWrittn, log);
			std::cout << "Shader Info Log: " << log << std::endl;
			free(log);
		}
	}


	void Renderer::PrintProgramLog(int prog) {
		int len = 0;
		int chWrittn = 0;
		char* log;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			log = (char*)malloc(len);
			glGetProgramInfoLog(prog, len, &chWrittn, log);
			std::cout << "Program Info Log: " << log << std::endl;
			free(log);
		}
	}


	bool Renderer::CheckOpenGLError() {
		bool foundError = false;
		int glErr = glGetError();
		while (glErr != GL_NO_ERROR) {
			std::cout << "glError: " << glErr << std::endl;
			foundError = true;
			glErr = glGetError();
		}
		return foundError;
	}

	GLuint Renderer::LoadTexture(const char* textureImagePath) {
		GLuint texture;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, numChannels;
		unsigned char* rawImgData = stbi_load(textureImagePath, &width, &height, &numChannels, 4);

		if (rawImgData) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImgData);
			glGenerateMipmap(GL_TEXTURE_2D);

			// if also anisotropic filtering
			if (("GL_EXT_texture_filter_anisotropic")) {
				GLfloat anisoSetting = 0.0f;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
			}
		}
		else {
			throw std::exception("Failed to load texture!");
		}

		stbi_image_free(rawImgData);

		return texture;
	}


	Renderer::Renderer() {
		renderingProgram = CreateShaderProgram(vShaderPath, fShaderPath);
		cam.Position() = { 0, 2, 4, 0 };
		cubeP = { 0.0f, 0.0f, 0.0f, 0.0f };

		textureAtlas = LoadTexture(textureAtlasPath);

		CreatePMat();

		SetupVertices();

		using namespace GW;
		using namespace GW::CORE;
		using namespace GW::SYSTEM;

		glfwSetWindowSizeCallback(window.window, framebuffer_size_callback);
	}

	

	void Renderer::Update(double dt) {
		glfwPollEvents();

		if (window.IsFocus()) {
			window.PollInput();
		}

		float speed = 5.0f;
		float mouseSensitivity = 0.05f;

		// --- horizontal movement --- //
		Vec4 moveDirection = cam.GetRightAxis() * window.GetInputAxisSide() + cam.GetForwardAxis() * window.GetInputAxisFwd();
		moveDirection = Normalize(moveDirection);
		cam.Position() += moveDirection * speed * dt;

		// --- vertical movement --- //
		float verticalMoveDirection = window.GetInputAxisUp();
		cam.Position()[1] += verticalMoveDirection * speed * dt;

		// --- mouse look --- //
		Vec2 mouseDelta = window.GetMouseDelta();
		cam.Rotation()[0] += mouseDelta[1] * mouseSensitivity;
		cam.Rotation()[1] += mouseDelta[0] * mouseSensitivity;

		// --- mouse scroll --- //
		if (window.GetKey(G_MOUSE_SCROLL_UP)) {
			FOV--;
			CreatePMat();
		}

		if (window.GetKey(G_MOUSE_SCROLL_DOWN)) {
			FOV++;
			CreatePMat();
		}


		cam.Update();
	}

	void Renderer::Draw(double dt) {
		glClearColor((70.0f / 255), (160.0f / 255), (255.0f / 255), (255.0f / 255));
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(renderingProgram);

		if (resized) {
			resized = false;
			CreatePMat();
		}

		// draw cube (buffer 0)
		cubeMMat = MakeTranslationMatrix(cubeP);
		mvMat = cam.VMat() * cubeMMat;

		mvMatLocation = glGetUniformLocation(renderingProgram, "mvMat");
		pMatLocation = glGetUniformLocation(renderingProgram, "pMat");

		glUniformMatrix4fv(mvMatLocation, 1, GL_FALSE, mvMat.data());
		glUniformMatrix4fv(pMatLocation, 1, GL_FALSE, cam.PMat().data());

		// verts
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// tex coords
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureAtlas);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window.window);
	}

	void Renderer::SetupVertices() {
		float cubeVerts[108] = {
			0, 0, -1,  1, 1, -1,  1, 0, -1,   0, 0, -1,  0, 1, -1,  1, 1, -1, // north
			0, 0,  0,  1, 0,  0,  0, 1,  0,   1, 0,  0,  1, 1,  0,  0, 1,  0, // south
			1, 0,  0,  1, 0, -1,  1, 1,  0,   1, 0, -1,  1, 1, -1,  1, 1,  0, // east
			0, 0, -1,  0, 0,  0,  0, 1, -1,   0, 0,  0,  0, 1,  0,  0, 1, -1, // west
			0, 1,  0,  1, 1,  0,  1, 1, -1,   0, 1,  0,  1, 1, -1,  0, 1, -1, // top
			1, 0,  0,  0, 0, -1,  1, 0, -1,   1, 0,  0,  0, 0,  0,  0, 0, -1 // bot
		};

		float cubeTexCoords[72] = {
			2, 1,  1, 0,  1, 1,   2, 1,  2, 0,  1, 0, // north
			1, 1,  2, 1,  1, 0,   2, 1,  2, 0,  1, 0, // south
			1, 1,  2, 1,  1, 0,   2, 1,  2, 0,  1, 0, // east
			1, 1,  2, 1,  1, 0,   2, 1,  2, 0,  1, 0, // west
			0, 1,  1, 1,  1, 0,   0, 1,  1, 0,  0, 0, // top
			3, 0,  2, 1,  3, 1,   3, 0,  2, 0,  2, 1 // bot
		};

		// textureAtlas is 64x64 blocks
		float unit = 1.0 / 64.0;
		for (size_t i = 0; i < 72; i++) {
			cubeTexCoords[i] *= unit;
		}

		glGenVertexArrays(numVAOs, vao);
		glBindVertexArray(vao[0]);
		glGenBuffers(numVBOs, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
	}
}