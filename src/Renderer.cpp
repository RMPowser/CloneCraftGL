#include "Renderer.h"
#include "OpenGL.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define vShaderPath "./shaders/vShader.glsl"
#define fShaderPath "./shaders/fShader.glsl"
#define textureAtlasPath "./textures/textureAtlas.png"
#define modelPath "./models/model.obj"



float FOV = 90;
bool resized = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	resized = true;
}

namespace CC {
	void Renderer::CreatePMat() {
		cam.PMat() = MakeProjectionMatrix(FOV * RADIAN, window.GetAspectRatio(), 0.01f, 1000.0f);
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

		glValidateProgram(vfProgram);

		glDeleteShader(vShader);
		glDeleteShader(fShader);

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
		stbi_set_flip_vertically_on_load(true);
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
		model.mMat.Position() = { 0.0f, 0.0f, 0.0f, 0.0f };

		textureAtlas = LoadTexture(textureAtlasPath);

		CreatePMat();

		globalAmbient = { 0.7, 0.7, 0.7, 1.0 };
		light.ambient = { 0.0, 0.0, 0.0, 1.0 };
		light.diffuse = { 1.0, 1.0, 1.0, 1.0 };
		light.specular = { 1.0, 1.0, 1.0, 1.0 };
		light.position = { 5.0, 2.0, 2.0 };

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

		mvMat = cam.VMat() * model.mMat;
		invTrns_mvMat = Transpose(Inverse(mvMat));

		mvMatLoc = glGetUniformLocation(renderingProgram, "mvMat");
		pMatLoc = glGetUniformLocation(renderingProgram, "pMat");
		nMatLoc = glGetUniformLocation(renderingProgram, "nMat");

		glUniformMatrix4fv(mvMatLoc, 1, GL_FALSE, mvMat.data());
		glUniformMatrix4fv(pMatLoc, 1, GL_FALSE, cam.PMat().data());
		glUniformMatrix4fv(nMatLoc, 1, GL_FALSE, invTrns_mvMat.data());

		SetupLightData(cam.VMat());

		// verts
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(0);

		CheckOpenGLError();

		// normals
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		CheckOpenGLError();

		// texture coodinates
		//glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void* )offsetof(Vertex, texCoord));
		//glEnableVertexAttribArray(2);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, textureAtlas);

		// indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);

		CheckOpenGLError();

		//glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, 0);
		CheckOpenGLError();

		if (CheckOpenGLError()) {
			PrintProgramLog(renderingProgram);
		}

		glfwSwapBuffers(window.window);
	}

	Model Renderer::LoadModel(const char* modelFilePath) {
		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(modelFilePath)) {
			if (!reader.Error().empty()) {
				std::cerr << "TinyObjReader: " << reader.Error();
				throw std::exception("failed to load obj file!");
			}
			throw std::exception("failed to load obj file!");
		}

		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning();
		}

		Model m;
		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
					tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
					tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

					tinyobj::real_t nx;
					tinyobj::real_t ny;
					tinyobj::real_t nz;
					if (attrib.normals.size() != 0) {
						nx = attrib.normals[3 * idx.normal_index + 0];
						ny = attrib.normals[3 * idx.normal_index + 1];
						nz = attrib.normals[3 * idx.normal_index + 2];
					}
					else {
						nx = ny = nz = 0;
					}
					
					tinyobj::real_t tx;
					tinyobj::real_t ty;
					if (attrib.texcoords.size() != 0) {
						tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}
					else {
						tx = ty = 0;
					}

					m.indices.push_back(m.vertices.size());
					m.vertices.push_back({ {vx, vy, vz}, {nx, ny, nz}, {tx, ty} });
				}
				index_offset += fv;
			}
		}

		return m;
	}

	void Renderer::SetupVertices() {
		model = LoadModel(modelPath);
		model.material = Materials::gold;



		glGenVertexArrays(numVAOs, vao);
		glBindVertexArray(vao[0]);

		glGenBuffers(numVBOs, vbo);

		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

		// normals
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

		// texture coordinates
		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

		// indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), model.indices.data(), GL_STATIC_DRAW);

		CheckOpenGLError();
	}

	void Renderer::SetupLightData(Mat4 vMatrix) {
		// convert lights position to view space
		Vec3 lightPosViewSpace(vMatrix * Vec4(light.position, 1.0));

		// get locations of light and material fields in shader
		L_GlobalAmbLoc	= glGetUniformLocation(renderingProgram, "globalAmbient");
		L_AmbLoc		= glGetUniformLocation(renderingProgram, "light.ambient");
		L_DiffLoc		= glGetUniformLocation(renderingProgram, "light.diffuse");
		L_SpecLoc		= glGetUniformLocation(renderingProgram, "light.specular");
		L_PosLoc		= glGetUniformLocation(renderingProgram, "light.position");
		M_AmbLoc		= glGetUniformLocation(renderingProgram, "material.ambient");
		M_DiffLoc		= glGetUniformLocation(renderingProgram, "material.diffuse");
		M_SpecLoc		= glGetUniformLocation(renderingProgram, "material.specular");
		M_ShiLoc		= glGetUniformLocation(renderingProgram, "material.shininess");

		// set the fields in the shader
		glProgramUniform4fv(renderingProgram, L_GlobalAmbLoc, 1, &globalAmbient[0]);
		glProgramUniform4fv(renderingProgram, L_AmbLoc, 1, &light.ambient[0]);
		glProgramUniform4fv(renderingProgram, L_DiffLoc, 1, &light.diffuse[0]);
		glProgramUniform4fv(renderingProgram, L_SpecLoc, 1, &light.specular[0]);
		glProgramUniform3fv(renderingProgram, L_PosLoc, 1, &light.position[0]);
		glProgramUniform4fv(renderingProgram, M_AmbLoc, 1, &model.material.ambient[0]);
		glProgramUniform4fv(renderingProgram, M_DiffLoc, 1, &model.material.diffuse[0]);
		glProgramUniform4fv(renderingProgram, M_SpecLoc, 1, &model.material.specular[0]);
		glProgramUniform1f(renderingProgram, M_ShiLoc, model.material.shininess);
	}
}