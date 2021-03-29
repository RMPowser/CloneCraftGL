#pragma once

#include "Math.hpp"
#include <vector>
#include <string>

class CubeMap {
private:
	unsigned int id;
	std::vector<std::string> filePaths;
	
public:
	bool isValid = false;

	CubeMap();
	~CubeMap();

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void Load(const std::vector<std::string>& filePaths);
};