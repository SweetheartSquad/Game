#version 150

in vec3 aVertexPosition;
in vec4 aVertexColor;
in vec3 aVertexNormals;
in vec2 aVertexUVs;

out vec2 Texcoord;

void main() {
    Texcoord = aVertexUVs;
    gl_Position = vec4(aVertexPosition, 1.0);
}