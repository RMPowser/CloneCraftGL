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
	void SetClearColor(float r, float g, float b, float a);
	void ClearScreen();
	void DrawIndexed(const VertexBufferArray& vb, const IndexBuffer& ib, const ShaderProgram& shader);
	void DrawIndexedInstanced(const VertexBufferArray& vba, const IndexBuffer& ib, const ShaderProgram& shader, unsigned int instanceCount);
	void SwapBuffers();
	void LoadModel(const char* modelFilePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const;
};