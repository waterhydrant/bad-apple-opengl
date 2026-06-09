#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform vec2 uScale;

void main() {
    TexCoord = texCoord;
    gl_Position = vec4(pos * uScale, 0.0, 1.0);
}
