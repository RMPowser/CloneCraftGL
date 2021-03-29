#include "Texture.h"
#include "OpenGL.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb/stb_image.h"

Texture::Texture() {
	GLErrorCheck(glGenTextures(1, &id));
}

Texture::~Texture() {
	GLErrorCheck(glDeleteTextures(1, &id));
}

void Texture::Bind(unsigned int slot /*= 0*/) const {
	GLErrorCheck(glActiveTexture(GL_TEXTURE0 + slot));
	GLErrorCheck(glBindTexture(GL_TEXTURE_2D, id));
}

void Texture::UnBind() const {
	GLErrorCheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::Load(const std::string& _filePath) {
	filePath = _filePath;
	stbi_set_flip_vertically_on_load(true);

	GLErrorCheck(glBindTexture(GL_TEXTURE_2D, id));

	GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR));
	GLErrorCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

	localBuffer = stbi_load(filePath.c_str(), &width, &height, &bitsPerPixel, 4);

	if (localBuffer) {
		GLErrorCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
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

	GLErrorCheck(glBindTexture(GL_TEXTURE_2D, 0));

	if (localBuffer) {
		stbi_image_free(localBuffer);
	}

	isValid = true;
}