#pragma once

#include "Math.hpp"
#include <string>
#include <unordered_map>


class ShaderProgram {
private:
	std::string filePath;	// for debug purposes only
	int id;			// the id for this specific shader program
	// caching for uniforms
	std::unordered_map<std::string, int> uniformLocationCache;

	struct ShaderProgramSource {
		std::string vertexSource;
		std::string fragmentSource;
	};

public:
	ShaderProgram(const std::string& filePath);
	~ShaderProgram();

	void Bind() const;
	void UnBind() const;

	// set uniforms
	void SetUniformMatrix4fv(const std::string& name, const Mat4& m);

private:
	ShaderProgramSource ParseShader(const std::string filePath) const;
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	unsigned int CreateShaderProgram(const std::string& vShader, const std::string& fShader) const;

	int GetUniformLocation(const std::string& name);
};
