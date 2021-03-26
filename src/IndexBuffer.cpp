#include "IndexBuffer.h"
#include "OpenGL.hpp"
#include <cassert>


IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) 
	: count(count) {
	assert(sizeof(unsigned int) == sizeof(GLuint));

	GLErrorCheck(glGenBuffers(1, &id));
	GLErrorCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
	GLErrorCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer() {
	GLErrorCheck(glDeleteBuffers(1, &id));
}

void IndexBuffer::Bind() const {
	GLErrorCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
}

void IndexBuffer::UnBind() const  {
	GLErrorCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}