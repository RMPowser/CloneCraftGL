#include "VertexBufferArray.h"

#include "OpenGL.hpp"
#include <unordered_map>

static std::unordered_map<unsigned int, bool> attribPointersCache; // for keeping track of which pointers are enabled

VertexBufferArray::VertexBufferArray() {
	GLErrorCheck(glGenVertexArrays(1, &id));
}

VertexBufferArray::~VertexBufferArray() {
	GLErrorCheck(glDeleteVertexArrays(1, &id));
	for (size_t i = 0; i < attribPointersCache.size(); i++) {
		attribPointersCache[i] = false;
	}
}

void VertexBufferArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	unsigned int attribIndex = 0;

	while (attribPointersCache[attribIndex]) {
		attribIndex++;
	}

	for (size_t i = 0; i < elements.size(); i++, attribIndex++) {
		const auto& element = elements[i];
		GLErrorCheck(glEnableVertexAttribArray(attribIndex));
		GLErrorCheck(glVertexAttribPointer(attribIndex, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
		attribPointersCache[attribIndex] = true;
		glVertexAttribDivisor(attribIndex, element.divisor);
		offset += element.count * element.GetSizeOfType(element.type);
	}
}

void VertexBufferArray::Bind() const {
	GLErrorCheck(glBindVertexArray(id));
}

void VertexBufferArray::UnBind() const {
	GLErrorCheck(glBindVertexArray(0));
}