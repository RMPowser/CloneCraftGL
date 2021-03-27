#include "ShaderProgram.h"
#include "OpenGL.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


ShaderProgram::ShaderProgram() {
	GLErrorCheck(id = glCreateProgram());
}

ShaderProgram::~ShaderProgram() {
	GLErrorCheck(glDeleteProgram(id));
}

void ShaderProgram::Bind() const {
	GLErrorCheck(glUseProgram(id));
}

void ShaderProgram::UnBind() const {
	GLErrorCheck(glUseProgram(0));
}

void ShaderProgram::Load(const std::string& _filePath) {
	filePath = _filePath;
	ShaderProgramSource source = ParseShader(_filePath);
	CreateShaderProgram(source.vertexSource, source.geometrySource, source.fragmentSource);
	isValid = true;
}

void ShaderProgram::SetUniform1i(const std::string& name, int value) {
	GLErrorCheck(glUniform1i(GetUniformLocation(name), value));
}

void ShaderProgram::SetUniformMatrix4fv(const std::string& name, const Mat4& m) {
	GLErrorCheck(glUniformMatrix4fv(GetUniformLocation(name), 1, false, m.data()));
}

int ShaderProgram::GetUniformLocation(const std::string& name) {
	if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
		return uniformLocationCache[name];
	}

	GLErrorCheck(int location = glGetUniformLocation(id, name.c_str()));
	if (location == -1) {
		std::cout << "Warning: uniform '" << name << "' not found!" << std::endl;
	}

	uniformLocationCache[name] = location;
	return location;
}

ShaderProgram::ShaderProgramSource ShaderProgram::ParseShader(const std::string filePath) const {
	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		GEOMETRY = 1,
		FRAGMENT = 2
	};

	std::ifstream fileStream(filePath);

	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;

	while (getline(fileStream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("geometry") != std::string::npos)
				type = ShaderType::GEOMETRY;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	fileStream.close();
	return { ss[0].str(), ss[1].str(), ss[2].str() };
}

unsigned int ShaderProgram::CompileShader(unsigned int type, const std::string& source) const {
	GLErrorCheck(unsigned int shaderID = glCreateShader(type));
	const char* src = source.c_str();
	GLErrorCheck(glShaderSource(shaderID, 1, &src, nullptr));
	GLErrorCheck(glCompileShader(shaderID));

	int result;
	GLErrorCheck(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result));
	if (result == false) {
		std::string shaderType;
		switch (type) {
			case GL_VERTEX_SHADER:
				shaderType = "vertex";
				break;
			case GL_GEOMETRY_SHADER:
				shaderType = "geometry";
				break;
			case GL_FRAGMENT_SHADER:
				shaderType = "fragment";
				break;
		}

		std::cout << "Failed to compile " << shaderType << " shader!" << std::endl;
		
		PrintShaderLog(shaderID);

		GLErrorCheck(glDeleteShader(shaderID));
		return 0;
	}

	return shaderID;
}

void ShaderProgram::CreateShaderProgram(const std::string& vShader, const std::string& gShader, const std::string& fShader) const {
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vShader);
	GLErrorCheck(glAttachShader(id, vs));

	unsigned int gs = 0;
	if (gShader != "") {
		gs = CompileShader(GL_GEOMETRY_SHADER, gShader);
		GLErrorCheck(glAttachShader(id, gs));
	}

	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fShader);
	GLErrorCheck(glAttachShader(id, fs));



	GLErrorCheck(glLinkProgram(id));
	int result;
	GLErrorCheck(glGetProgramiv(id, GL_LINK_STATUS, &result));
	if (result != 1) {
		std::cout << "shader linking failed!" << std::endl;
		PrintProgramLog();
		GLErrorCheck(glDeleteProgram(id));
	}

	GLErrorCheck(glValidateProgram(id));

	GLErrorCheck(glDeleteShader(vs));
	if (gs != 0) { GLErrorCheck(glDeleteShader(gs)); }
	GLErrorCheck(glDeleteShader(fs));
}

void ShaderProgram::PrintShaderLog(unsigned int id) const {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(id, len, &chWrittn, log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}
}

void ShaderProgram::PrintProgramLog() const {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(id, len, &chWrittn, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}
}