#include "VertexBuffer.h"
#include "OpenGL.hpp"


VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
	GLErrorCheck(glGenBuffers(1, &id));
	GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, id));
	GLErrorCheck(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer() {
	GLErrorCheck(glDeleteBuffers(1, &id));
}

void VertexBuffer::Bind() const {
	GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::UnBind() const {
	GLErrorCheck(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
