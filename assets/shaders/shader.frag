#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D videoTexture;

void main() {
    float rle_pixel = texture(videoTexture, TexCoord).r;

    FragColor = vec4(vec3(rle_pixel), 1.0);
}
