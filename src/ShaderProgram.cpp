#include "ShaderProgram.h"
#include "Renderer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


namespace CC {
	ShaderProgram::ShaderProgram(const std::string& filePath) 
		: filePath(filePath) {
		ShaderProgramSource source = ParseShader(filePath);
		programID = CreateShaderProgram(source.vertexSource, source.fragmentSource);
	}

	ShaderProgram::~ShaderProgram() {
		GLErrorCheck(glDeleteProgram(programID));
	}

	void ShaderProgram::Bind() const {
		GLErrorCheck(glUseProgram(programID));
	}

	void ShaderProgram::UnBind() const {
		GLErrorCheck(glUseProgram(0));
	}

	void ShaderProgram::SetUniformMatrix4fv(const std::string& name, const Mat4& m) {
		GLErrorCheck(glUniformMatrix4fv(GetUniformLocation(name), 1, false, m.data()));
	}

	int ShaderProgram::GetUniformLocation(const std::string& name) {
		if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
			return uniformLocationCache[name];
		}

		GLErrorCheck(int location = glGetUniformLocation(programID, name.c_str()));
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
			FRAGMENT = 1
		};

		std::ifstream fileStream(filePath);

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;

		while (getline(fileStream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}

		fileStream.close();
		return { ss[0].str(), ss[1].str() };
	}

	unsigned int ShaderProgram::CompileShader(unsigned int type, const std::string& source) const {
		GLErrorCheck(unsigned int id = glCreateShader(type));
		const char* src = source.c_str();
		GLErrorCheck(glShaderSource(id, 1, &src, nullptr));
		GLErrorCheck(glCompileShader(id));

		int result;
		GLErrorCheck(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		if (result == false) {
			int length;
			GLErrorCheck(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
			char* message = (char*)alloca(length * sizeof(char));
			GLErrorCheck(glGetShaderInfoLog(id, length, &length, message));
			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
			std::cout << message << std::endl;
			GLErrorCheck(glDeleteShader(id));
			return 0;
		}

		return id;
	}

	unsigned int ShaderProgram::CreateShaderProgram(const std::string& vShader, const std::string& fShader) const {
		GLErrorCheck(unsigned int program = glCreateProgram());
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fShader);

		GLErrorCheck(glAttachShader(program, vs));
		GLErrorCheck(glAttachShader(program, fs));
		GLErrorCheck(glLinkProgram(program));
		GLErrorCheck(glValidateProgram(program));

		GLErrorCheck(glDeleteShader(vs));
		GLErrorCheck(glDeleteShader(fs));

		return program;
	}
}
