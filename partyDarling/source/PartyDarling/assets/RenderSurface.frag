#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform int reverse = 0;
uniform float transition = 0;
uniform float xMult = 0;
uniform float yMult = 0;
uniform vec3 wipeColour = vec3(0,0,0);

void main() {
	bool inTransition = Texcoord.x <= transition*xMult || Texcoord.y <= transition*yMult;
	if(reverse == 1){
		inTransition = !inTransition;
	}if(inTransition){
    	outColor = vec4(wipeColour, 1);
	}else{
    	outColor = vec4(texture(texFramebuffer, Texcoord).rgb, 1);
    }
}
