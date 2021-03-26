#shader vertex
#version 430 core

uniform mat4 mvMat;
uniform mat4 pMat;

layout (location=0) in vec4 vPos;
layout (location=1) in vec2 tCoord;
out vec2 texCoord;

void main() {
	gl_Position = pMat * mvMat * vPos;
	texCoord = tCoord;
}





#shader fragment
#version 430

in vec2 texCoord;
out vec4 pixelColor;

layout (binding=0) uniform sampler2D _texture;

void main() {
	pixelColor = texture(_texture, texCoord);
}