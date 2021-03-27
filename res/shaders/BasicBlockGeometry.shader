#shader vertex
#version 430 core

uniform mat4 mvMat;
uniform mat4 pMat;

layout (location=0) in vec4 vPos;

void main() {
	gl_Position = pMat * mvMat * vPos;
}



#shader geometry
#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 36) out;

out vec2 texCoord;

void main() {
	// cube vertices
	vec4 vertices[8] = vec4[8](
		vec4(0, 1, 0, 1),
		vec4(0, 0, 0, 1),
		vec4(1, 1, 0, 1),
		vec4(1, 0, 0, 1),
		vec4(0, 1, 1, 1),
		vec4(0, 0, 1, 1),
		vec4(1, 1, 1, 1),
		vec4(1, 0, 1, 1)
	);

	vec2 texCoords[22] = vec2[22](
		vec2(0.000000, 1.000000),
		vec2(1.000000, 0.666667),
		vec2(1.000000, 1.000000),
		vec2(0.999942, 0.000058),
		vec2(0.000058, 0.333353),
		vec2(0.000058, 0.000058),
		vec2(0.000000, 0.333333),
		vec2(1.000000, 0.333333),
		vec2(0.000000, 0.666667),
		vec2(1.000000, 0.333333),
		vec2(0.000000, 0.666667),
		vec2(0.999942, 0.333353),
		vec2(1.000000, 0.666667),
		vec2(0.000000, 0.333333),
		vec2(1.000000, 0.666667),
		vec2(0.000000, 0.666667),
		vec2(1.000000, 0.333333),
		vec2(0.000000, 0.333333),
		vec2(0.000000, 0.333333),
		vec2(1.000000, 0.333333),
		vec2(0.000000, 0.666667),
		vec2(1.000000, 0.666667)
	);

	// face 1
	gl_Position = gl_in[0].gl_Position + vertices[4];
	texCoord = texCoords[0];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertices[2];
	texCoord = texCoords[1];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertices[0];
	texCoord = texCoords[2];
	EmitVertex();
	EndPrimitive();

	gl_Position = gl_in[0].gl_Position + vertices[1];
	texCoord = texCoords[3];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertices[7];
	texCoord = texCoords[4];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vertices[5];
	texCoord = texCoords[5];
	EmitVertex();
	EndPrimitive();


	//1, 1, -1, 1, 0, -1, 0, 0, -1, 0, 1, -1, 1, 1, -1, // north
}



#shader fragment
#version 430 core

in vec2 texCoord;
out vec4 pixelColor;

layout (binding=0) uniform sampler2D _texture;

void main() {
	pixelColor = texture(_texture, texCoord);
}