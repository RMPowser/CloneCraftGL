#include "Renderer.h"
#include "OpenGL.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "TinyObjLoader/tiny_obj_loader.h"

#include <fstream>
#include <iostream>
#include <string>

Renderer::Renderer(Window& window) : window(window) {
	glfwSwapInterval(1); // vsync
	GLErrorCheck(glEnable(GL_CULL_FACE));
	GLErrorCheck(glFrontFace(GL_CCW));
	GLErrorCheck(glEnable(GL_DEPTH_TEST));
	GLErrorCheck(glDepthFunc(GL_LEQUAL));
	GLErrorCheck(glEnable(GL_BLEND));
	GLErrorCheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	SetClearColor((70.0f / 255), (160.0f / 255), (255.0f / 255), (255.0f / 255));
}

void Renderer::SetClearColor(float r, float g, float b, float a) {
	GLErrorCheck(glClearColor(r, g, b, a));
}

void Renderer::ClearScreen() {
	GLErrorCheck(glClear(GL_DEPTH_BUFFER_BIT));
	GLErrorCheck(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::DrawIndexed(const VertexBufferArray& vba, const IndexBuffer& ib, const ShaderProgram& shader) {
	vba.Bind();
	shader.Bind();
	
	GLErrorCheck(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::DrawIndexedInstanced(const VertexBufferArray& vba, const IndexBuffer& ib, const ShaderProgram& shader, unsigned int instanceCount) {
	vba.Bind();
	shader.Bind();

	glDrawElementsInstanced(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0, instanceCount);
}

void Renderer::SwapBuffers() {
	glfwSwapBuffers(window.glfwWindow);
}

void Renderer::LoadModel(const char* modelFilePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const {
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(modelFilePath)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
			throw std::exception("failed to load obj file!");
		}
		throw std::exception("failed to load obj file!");
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];

				tinyobj::real_t nx;
				tinyobj::real_t ny;
				tinyobj::real_t nz;
				if (attrib.normals.size() != 0) {
					nx = attrib.normals[3 * idx.normal_index + 0];
					ny = attrib.normals[3 * idx.normal_index + 1];
					nz = attrib.normals[3 * idx.normal_index + 2];
				}
				else {
					nx = ny = nz = 0;
				}

				tinyobj::real_t tx;
				tinyobj::real_t ty;
				if (attrib.texcoords.size() != 0) {
					tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				}
				else {
					tx = ty = 0;
				}

				indices.push_back(vertices.size());
				vertices.push_back({ {vx, vy, vz}, {tx, ty} });
			}
			index_offset += fv;
		}
	}
}