#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float center = 0.2f;
uniform float amount = 0.1f;


vec2 distort(vec2 uv){
	vec2 newUv = uv;

	//float center = 0.2f;
	//float amount = 0.1f;
	if(mod(uv.x, 1.f) > center - amount && mod(uv.x, 1.f) < center + amount){
		newUv.y += amount - abs(mod(uv.x, 1.f) - center);
	}
	return newUv;
	return uv;
}

void main() {
    outColor = vec4(texture(texFramebuffer, distort(Texcoord)));
}