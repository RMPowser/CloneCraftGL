#shader vertex
#version 430 core

uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

void main() {
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1);
    gl_Position = pos.xyww;
}





#shader fragment
#version 430

uniform samplerCube skybox;

in vec3 TexCoords;

out vec4 FragColor;

void main() {
    FragColor = texture(skybox, TexCoords);
}