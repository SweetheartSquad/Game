#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform int reverse = 0;
uniform float transition = 0;
uniform float xMult = 0;
uniform float yMult = 0;
uniform vec3 wipeColour = vec3(0,0,0);


vec4 distort(vec4 _color){
	vec4 res = _color;

	float f = fract((_color.r + _color.g + _color.b)*2);
	float d1 = (f - _color.r);
	float d2 = (f - _color.g);
	float d3 = (f - _color.b);
	float maxD = max(abs(d1), max(abs(d2), abs(d3)));

	if(maxD == abs(d1)){
	res.r += wipeColour.r * d1;
	}if(maxD == abs(d2)){
	res.g += wipeColour.g * d2;
	}if(maxD == abs(d3)){
	res.b += wipeColour.b * d3;
	}
	res = mix(res, _color, 0.75);
	return res;
}

void main() {
	vec4 col = vec4(texture(texFramebuffer, Texcoord).rgb, 1);
	col = distort(col);

	bool inTransition = Texcoord.x <= transition*xMult || Texcoord.y <= transition*yMult;
	if(reverse == 1){
		inTransition = !inTransition;
	}if(inTransition){
    	outColor = vec4(1) - col;//vec4(wipeColour, 1);
	}else{
    	outColor = col;
    }


    
}



// rejected distortions
/*
// fake outlines
float d = 0.001;
float d2 = 0.01;
vec4 col = vec4(texture(texFramebuffer, Texcoord).rgb, 1);
vec4 off1 = texture(texFramebuffer, Texcoord + vec2(d));
vec4 off2 = texture(texFramebuffer, Texcoord - vec2(d));

float boop = max(length(col - off1), length(col - off2));
if(boop > d2){
    col = col * (0.5f-boop);//vec4(1) - col * 0.5f;
    //discard;
}
*/

/*
// disco
vec4 col = vec4(texture(texFramebuffer, Texcoord).rgb, 1);
vec4 col2 = col;
if(col.r < col.g || col.r < col.b){
	col2.r = fract(col.g + col.b);
}if(col.g < col.r || col.g < col.b){
	col2.g = fract(col.r + col.b);
}if(col.b < col.r || col.b < col.g){
	col2.b = fract(col.g + col.r);
}
col = mix(col2, col, 0);
 */