#version 430 core

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
	fragColor = vec4((ambient + diffuse + specular), 1.0);
}