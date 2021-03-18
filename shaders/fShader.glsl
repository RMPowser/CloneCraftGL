#version 430

in vec2 tc;
out vec4 color;

uniform mat4 mvMat;
uniform mat4 pMat;
layout (binding=0) uniform sampler2D samp;

void main() {
	color = texture(samp, tc);
}