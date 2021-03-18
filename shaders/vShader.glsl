#version 430

mat4 BuildTranslateMatrix(float x, float y, float z) {
	mat4 trans = mat4(1.0, 0.0, 0.0, 0.0,
					  0.0, 1.0, 0.0, 0.0,
					  0.0, 0.0, 1.0, 0.0,
					    x,   y,   z, 1.0);
	return trans;
}

mat4 BuildRotateXMatrix(float rad) {
	mat4 rotX = mat4(1.0,      0.0,       0.0, 0.0,
					 0.0, cos(rad), -sin(rad), 0.0,
					 0.0, sin(rad),  cos(rad), 0.0,
					 0.0,      0.0,       0.0, 1.0);
	return rotX;
}

mat4 BuildRotateYMatrix(float rad) {
	mat4 rotY = mat4( cos(rad), 0.0, sin(rad), 0.0,
					       0.0, 1.0,      0.0, 0.0,
					 -sin(rad), 0.0, cos(rad), 0.0,
					       0.0, 0.0,      0.0, 1.0);
	return rotY;
}

mat4 BuildRotateZMatrix(float rad) {
	mat4 rotZ = mat4( cos(rad), -sin(rad), 0.0, 0.0,
					  sin(rad),  cos(rad), 0.0, 0.0,
					 	   0.0,  	  0.0, 1.0, 0.0,
					   	   0.0,  	  0.0, 0.0, 1.0);
	return rotZ;
}

mat4 BuildScaleMatrix(float x, float y, float z) {
	mat4 scale = mat4(  x, 0.0, 0.0, 0.0,
					  0.0,   y, 0.0, 0.0,
					  0.0, 0.0,   z, 0.0,
					  0.0, 0.0, 0.0, 1.0);
	return scale;
}


layout (location=0) in vec3 position;
layout (location=1) in vec2 texCoord;
out vec2 tc;

uniform mat4 mvMat;
uniform mat4 pMat;
layout (binding=0) uniform sampler2D samp; // not used here in vShader

void main() {
	gl_Position = pMat * mvMat * vec4(position, 1.0);
	tc = texCoord;
}