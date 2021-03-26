#include "VertexBufferArray.h"

#include "OpenGL.hpp"

VertexBufferArray::VertexBufferArray() {
	GLErrorCheck(glGenVertexArrays(1, &id));
}

VertexBufferArray::~VertexBufferArray() {
	GLErrorCheck(glDeleteVertexArrays(1, &id));
}

void VertexBufferArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (size_t i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLErrorCheck(glEnableVertexAttribArray(i));
		GLErrorCheck(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset));
		offset += element.count * element.GetSizeOfType(element.type);
	}
}

void VertexBufferArray::Bind() const {
	GLErrorCheck(glBindVertexArray(id));
}

void VertexBufferArray::UnBind() const {
	GLErrorCheck(glBindVertexArray(0));
}
