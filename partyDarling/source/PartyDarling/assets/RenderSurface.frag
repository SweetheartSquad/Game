#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float transition = 0;
uniform vec3 wipeColour = vec3(0,0,0);

void main() {
	if(Texcoord.x <= 1-transition){
    	outColor = vec4(wipeColour, 1);
	}else{
    	outColor = vec4(texture(texFramebuffer, Texcoord).rgb, 1);
    }
}
