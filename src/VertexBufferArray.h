#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexBufferArray {
private:
	unsigned int id;

public:
	VertexBufferArray();
	~VertexBufferArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void UnBind() const;
};

