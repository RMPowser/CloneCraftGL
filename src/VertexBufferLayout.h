#pragma once

#include "OpenGL.hpp"
#include <vector>

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	bool normalized;
	unsigned int divisor = 0;

	static unsigned int GetSizeOfType(unsigned int GL_type) {
		switch (GL_type) {
			case GL_FLOAT:
				return 4;
			case GL_UNSIGNED_INT:
				return 4;
			case GL_UNSIGNED_BYTE:
				return 1;
		}
		__debugbreak();
		return 0;
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> elements;
	unsigned int stride = 0;
public:
	VertexBufferLayout() {
	
	}

	template<typename T>
	void Push(unsigned int count, bool normalized, unsigned int divisor) {
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count, bool normalized, unsigned int divisor) {
		elements.push_back({ GL_FLOAT, count, normalized, divisor });
		stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count, bool normalized, unsigned int divisor) {
		elements.push_back({ GL_UNSIGNED_INT, count, normalized, divisor });
		stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count, bool normalized, unsigned int divisor) {
		elements.push_back({ GL_UNSIGNED_BYTE, count, normalized, divisor });
		stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return elements; }
	inline unsigned int GetStride() const { return stride; }
};
