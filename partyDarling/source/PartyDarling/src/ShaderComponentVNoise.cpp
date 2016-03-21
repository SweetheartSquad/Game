#pragma once 

#include <ShaderComponentVNoise.h>
#include <shader/ShaderVariables.h>
#include <shader/ComponentShaderBase.h>
#include <MatrixStack.h>

#include <sweet.h>

ShaderComponentVNoise::ShaderComponentVNoise(ComponentShaderBase * _shader) :
	ShaderComponent(_shader),
	timeLocation(-1),
	mag1Location(-1),
	mag2Location(-1),
	mag1(0),
	mag2(0)/*,
	modelUniformLocation(-1),
	viewUniformLocation(-1),
	projectionUniformLocation(-1),
	mvpUniformLocation(-1)*/
{
}

ShaderComponentVNoise::~ShaderComponentVNoise() {
}

std::string ShaderComponentVNoise::getVertexVariablesString() {
	//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
	return
	"uniform float time = 0;"
	"uniform float magnitude1 = 0;"
	"uniform float magnitude2 = 0;"
	"float rand(vec2 co){"
		"return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);"
	"}" + SEMI_ENDL;
}

std::string ShaderComponentVNoise::getFragmentVariablesString() {
	return "";
}

std::string ShaderComponentVNoise::getVertexBodyString() {
	return 
		TAB + "fragUV += magnitude2 * vec2(rand(vec2(aVertexUVs.x, time)), rand(vec2(aVertexUVs.y, time)))" + SEMI_ENDL +
		TAB + "gl_Position += magnitude1 * vec4(rand(vec2(aVertexPosition.x, time)), rand(vec2(aVertexPosition.y, time)), rand(vec2(aVertexPosition.z, time)), 0)" + SEMI_ENDL;
}

std::string ShaderComponentVNoise::getFragmentBodyString() {
	return "";
}

std::string ShaderComponentVNoise::getOutColorMod() {
	// Not responsible for any colour modification
	return "";
}

void ShaderComponentVNoise::load() {
	if(!loaded){
		timeLocation = glGetUniformLocation(shader->getProgramId(), "time");
		mag1Location = glGetUniformLocation(shader->getProgramId(), "magnitude1");
		mag2Location = glGetUniformLocation(shader->getProgramId(), "magnitude2");
	}
	ShaderComponent::load();
}

void ShaderComponentVNoise::unload() {
	if(loaded){
		timeLocation = -1;
		mag1Location = -1; 
		mag2Location = -1;
	}
	ShaderComponent::unload();
}

void ShaderComponentVNoise::configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) {
	//glUniform1f(timeLocation, sweet::lastTimestamp);
	/*const glm::mat4 * m = _matrixStack->getModelMatrix();
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &(*m)[0][0]);
	
	m = _matrixStack->getViewMatrix();

	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &(*m)[0][0]);
	
	m = _matrixStack->getProjectionMatrix();
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &(*m)[0][0]);

	m = _matrixStack->getMVP();
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &(*m)[0][0]);*/
}

void ShaderComponentVNoise::setMag(float _mag1, float _mag2, float _interpolation){
	mag1 += (_mag1 - mag1) * _interpolation;
	mag2 += (_mag2 - mag2) * _interpolation;
}