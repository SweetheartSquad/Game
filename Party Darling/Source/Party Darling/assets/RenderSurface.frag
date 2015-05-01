#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;

void main() {
    outColor = vec4(texture(texFramebuffer, Texcoord));
    outColor.r +=0.2;
    outColor.a += 0.5;
}
