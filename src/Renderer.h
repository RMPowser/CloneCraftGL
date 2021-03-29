#pragma once

#include "Window.h"
#include "Vertex.h"
#include "VertexBufferArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"

class Renderer {
private:
	Window& window;
		
public:
	Renderer(Window& window);
	void SetClearColor(float r, float g, float b, float a) const;
	void ClearScreen() const;
	void DrawArrays(const VertexBufferArray& vba, unsigned int count, const ShaderProgram& shader) const;
	void DrawIndexed(const VertexBufferArray& vba, const IndexBuffer& ib, const ShaderProgram& shader) const;
	void DrawIndexedInstanced(const VertexBufferArray& vba, const IndexBuffer& ib, const ShaderProgram& shader, unsigned int instanceCount) const;
	void SwapBuffers() const;
	void LoadModel(const char* modelFilePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const;
};