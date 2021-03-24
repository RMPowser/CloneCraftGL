#shader vertex
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

struct PositionalLight {
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
uniform PositionalLight light;
uniform Material material;
uniform mat4 mvMat;
uniform mat4 pMat;
uniform mat4 nMat; // for transforming normals

layout (location=0) in vec3 vPos;
layout (location=1) in vec3 vNorm;
layout (location=2) in vec2 tCoord;
out vec3 varyingVertPos; // vertex position in eye space
flat out vec3 varyingNormal; // eye-space vertex normal
out vec3 varyingLightDir; // vector pointing to the light
out vec3 varyingHalfVector;
out vec2 texCoord;

layout (binding=0) uniform sampler2D samp;

void main() {
	varyingVertPos = (mvMat * vec4(vPos, 1.0)).xyz;
	varyingNormal = (nMat * vec4(vNorm, 1.0)).xyz;
	varyingLightDir = light.position - varyingVertPos;
	varyingHalfVector = (varyingLightDir + (-varyingVertPos)).xyz;
	gl_Position = pMat * mvMat * vec4(vPos, 1.0);
	texCoord = tCoord;
}





#shader fragment
#version 430

struct PositionalLight {
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
uniform PositionalLight light;
uniform Material material;
uniform mat4 mvMat;
uniform mat4 pMat;
uniform mat4 nMat; // for transforming normals

in vec3 varyingVertPos;
flat in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingHalfVector;
in vec2 texCoord;
out vec4 fragColor;

layout (binding=0) uniform sampler2D samp;

void main() {
	vec3 V = normalize(-varyingVertPos);
	vec3 N = normalize(varyingNormal);
	vec3 L = normalize(varyingLightDir);
	vec3 H = normalize(varyingHalfVector);

	// get the angle between the light and the surface normal
	float cosTheta = dot(L, N);
	// angle between the normal and the halfway vector
	float cosPhi = dot(H, N);

	// compute ADS contributions (per pixel), and combine to build output color
	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
	vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess * 3.0);

	diffuse = (diffuse * 0.5) + (texture(samp, texCoord).xyz * 0.5);
	// fragColor = vec4((ambient + diffuse + specular), 1.0);
	fragColor = texture(samp, texCoord);
}