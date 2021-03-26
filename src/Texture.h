#pragma once

#include <string>

class Texture {
private:
	unsigned int id;
	std::string filePath;
	unsigned char* localBuffer;
	int width, height, bitsPerPixel;

public:
	bool isValid = false;

	Texture();
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void UnBind() const;

	void Load(const std::string& filePath);

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
};