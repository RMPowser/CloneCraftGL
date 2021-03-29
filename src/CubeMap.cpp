#include "CubeMap.h"
#include "OpenGL.hpp"


#include "stb/stb_image.h"

CubeMap::CubeMap() {
	GLErrorCheck(glGenTextures(1, &id));
}

CubeMap::~CubeMap() {
	GLErrorCheck(glDeleteTextures(1, &id));
}

void CubeMap::Bind(unsigned int slot /*= 0*/) const {
	GLErrorCheck(glActiveTexture(GL_TEXTURE0 + slot));
	GLErrorCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
}

void CubeMap::UnBind() const {
	GLErrorCheck(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void CubeMap::Load(const std::vector<std::string>& _filePaths) {
	filePaths.clear();

	for (unsigned int i = 0; i < _filePaths.size(); i++) {
		filePaths.push_back(_filePaths[i]);
	}

	stbi_set_flip_vertically_on_load(false);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, nChannels;
	for (unsigned int i = 0; i < _filePaths.size(); i++) {
		unsigned char* data = stbi_load(_filePaths[i].c_str(), &width, &height, &nChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap tex failed to load at path: " << _filePaths[i] << std::endl;
			stbi_image_free(data);
			ASSERT(false);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	isValid = true;
}
