#include "Renderer.h"
#include "OpenGL.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "stb/stb_image.h"
#include "TinyObjLoader/tiny_obj_loader.h"

#define ASSERT(x) if (x) __debugbreak();
#define GLErrorCheck(x) CheckOpenGLError();\
	x;\
	ASSERT(CheckOpenGLError())


#define vShaderPath "./shaders/vShader.glsl"
#define fShaderPath "./shaders/fShader.glsl"
#define textureAtlasPath "./textures/textureAtlas.png"
#define modelPath "./models/model.obj"



bool resized = true;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	resized = true;
}

static void GLClearError() {

}

namespace CC {
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


	bool Renderer::CheckOpenGLError() const {
		bool foundError = false;
		int glErr = glGetError();
		while (glErr != GL_NO_ERROR) {
			std::cout << "glError: " << glErr << std::endl;
			foundError = true;
			glErr = glGetError();
		}
		return foundError;
	}

	


	Renderer::Renderer(Window& window) : window(window) {
		renderingProgram = CreateShaderProgram(vShaderPath, fShaderPath);
		glfwSwapInterval(1); // vsync
		GLErrorCheck(glEnable(GL_CULL_FACE));
		GLErrorCheck(glFrontFace(GL_CCW));
		GLErrorCheck(glEnable(GL_DEPTH_TEST));
		GLErrorCheck(glDepthFunc(GL_LEQUAL));
		SetClearColor((70.0f / 255), (160.0f / 255), (255.0f / 255), (255.0f / 255));

		glfwSetWindowSizeCallback(window.glfwWindow, framebuffer_size_callback);

		
		GLErrorCheck(glGenBuffers(numVBOs, vbo));
	}

	void Renderer::SetClearColor(float r, float g, float b, float a) {
		GLErrorCheck(glClearColor(r, g, b, a));
	}

	void Renderer::ClearScreen() {
		GLErrorCheck(glClear(GL_DEPTH_BUFFER_BIT));
		GLErrorCheck(glClear(GL_COLOR_BUFFER_BIT));
	}

	void Renderer::SetActiveTexture(const unsigned int& texture) {
		GLErrorCheck(glActiveTexture(GL_TEXTURE0));
		GLErrorCheck(glBindTexture(GL_TEXTURE_2D, texture));
	}

	void Renderer::Draw(Camera& cam, const Model& model, const Mat4& mMat, const unsigned int& vao) {
		GLErrorCheck(glUseProgram(renderingProgram));

		if (resized) {
			resized = false;
			cam.RecreateProjectionMatrix(window.GetAspectRatio());
		}

		GLErrorCheck(glBindVertexArray(vao));

		//SetupLightData(cam.vMat, light, model);

		mvMat = cam.vMat * IdentityMatrix();
		nMat = Transpose(Inverse(mvMat));

		GLErrorCheck(mvMatLoc = glGetUniformLocation(renderingProgram, "mvMat"));
		GLErrorCheck(pMatLoc = glGetUniformLocation(renderingProgram, "pMat"));
		GLErrorCheck(nMatLoc = glGetUniformLocation(renderingProgram, "nMat"));

		GLErrorCheck(glUniformMatrix4fv(mvMatLoc, 1, GL_FALSE, mvMat.data()));
		GLErrorCheck(glUniformMatrix4fv(pMatLoc, 1, GL_FALSE, cam.pMat.data()));
		GLErrorCheck(glUniformMatrix4fv(nMatLoc, 1, GL_FALSE, nMat.data()));

		// verts
		GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, vbo[0]));
		GLErrorCheck(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position)));
		GLErrorCheck(glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW));
		GLErrorCheck(glEnableVertexAttribArray(0));
		// normals
		GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, vbo[1]));
		GLErrorCheck(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal)));
		GLErrorCheck(glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW));
		GLErrorCheck(glEnableVertexAttribArray(1));
		// texture coodinates
		GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, vbo[2]));
		GLErrorCheck(glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoord)));
		GLErrorCheck(glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW));
		GLErrorCheck(glEnableVertexAttribArray(2));
		// indices
		GLErrorCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]));
		GLErrorCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), model.indices.data(), GL_STATIC_DRAW));

		GLErrorCheck(glDrawElements(GL_LINES, model.indices.size(), GL_UNSIGNED_INT, 0));
	}

	void Renderer::SwapBuffers() {
		glfwSwapBuffers(window.glfwWindow);
	}

	void Renderer::SetupLightData(Mat4& vMatrix, PositionalLight& light, Model& model) {
		// convert lights position to view space
		Vec3 lightPosViewSpace(vMatrix * Vec4(light.position, 1.0));

		// get locations of light and material fields in shader
		GLErrorCheck(L_GlobalAmbLoc	= glGetUniformLocation(renderingProgram, "globalAmbient"));
		GLErrorCheck(L_AmbLoc		= glGetUniformLocation(renderingProgram, "light.ambient"));
		GLErrorCheck(L_DiffLoc		= glGetUniformLocation(renderingProgram, "light.diffuse"));
		GLErrorCheck(L_SpecLoc		= glGetUniformLocation(renderingProgram, "light.specular"));
		GLErrorCheck(L_PosLoc		= glGetUniformLocation(renderingProgram, "light.position"));
		GLErrorCheck(M_AmbLoc		= glGetUniformLocation(renderingProgram, "material.ambient"));
		GLErrorCheck(M_DiffLoc		= glGetUniformLocation(renderingProgram, "material.diffuse"));
		GLErrorCheck(M_SpecLoc		= glGetUniformLocation(renderingProgram, "material.specular"));
		GLErrorCheck(M_ShiLoc		= glGetUniformLocation(renderingProgram, "material.shininess"));

		// set the fields in the shader
		GLErrorCheck(glProgramUniform4fv(renderingProgram, L_GlobalAmbLoc, 1, &globalAmbient[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, L_AmbLoc, 1, &light.ambient[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, L_DiffLoc, 1, &light.diffuse[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, L_SpecLoc, 1, &light.specular[0]));
		GLErrorCheck(glProgramUniform3fv(renderingProgram, L_PosLoc, 1, &light.position[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, M_AmbLoc, 1, &model.material.ambient[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, M_DiffLoc, 1, &model.material.diffuse[0]));
		GLErrorCheck(glProgramUniform4fv(renderingProgram, M_SpecLoc, 1, &model.material.specular[0]));
		GLErrorCheck(glProgramUniform1f(renderingProgram, M_ShiLoc, model.material.shininess));
	}

	unsigned int Renderer::LoadTexture(const char* textureImagePath) const {
		unsigned int texture;

		GLErrorCheck(glGenTextures(1, &texture));
		GLErrorCheck(glBindTexture(GL_TEXTURE_2D, texture));

		GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
		GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

		int width, height, numChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* rawImgData = stbi_load(textureImagePath, &width, &height, &numChannels, 4);

		if (rawImgData) {
			GLErrorCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawImgData));
			GLErrorCheck(glGenerateMipmap(GL_TEXTURE_2D));

			// if also anisotropic filtering
			if (("GL_EXT_texture_filter_anisotropic")) {
				float anisoSetting = 0.0f;
				GLErrorCheck(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting));
				GLErrorCheck(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting));
			}
		}
		else {
			throw std::exception("Failed to load texture!");
		}

		stbi_image_free(rawImgData);

		return texture;
	}

	Model Renderer::LoadModel(const char* modelFilePath) const {
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
}