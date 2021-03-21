#version 430 core

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

struct Light {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform Light light;
uniform Material material;
uniform mat4 mvMat;
uniform mat4 pMat;
uniform mat4 nMat; // for transforming normals

layout (location=0) in vec3 vPos;
layout (location=1) in vec3 vNorm;
out vec3 varyingVertPos; // vertex position in eye space
out vec3 varyingNormal; // eye-space vertex normal
out vec3 varyingLightDir; // vector pointing to the light
out vec3 varyingHalfVector;

//layout (binding=0) uniform sampler2D samp; // not used here in vShader

void main() {
	varyingVertPos = (mvMat * vec4(vPos, 1.0)).xyz;
	varyingNormal = (nMat * vec4(vNorm, 1.0)).xyz;
	varyingLightDir = light.position - varyingVertPos;
	varyingHalfVector = (varyingLightDir + (-varyingVertPos)).xyz;
	gl_Position = pMat * mvMat * vec4(vPos, 1.0);
}