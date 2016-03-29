#pragma

#include "ShaderComponentMap.h"
#include "shader/ShaderVariables.h"
#include <shader\ComponentShaderBase.h>
#include "MatrixStack.h"
#include "RenderOptions.h"
#include "Texture.h"
#include "node/NodeRenderable.h"
#include "MeshInterface.h"
#include "Sprite.h"
#include "SpriteSheetAnimation.h"

ShaderComponentMap::ShaderComponentMap(ComponentShaderBase * _shader, float _alphaDiscardThreshold) :
	ShaderComponent(_shader),
	alphaDiscardThreshold(_alphaDiscardThreshold),
	texNumLoc(-1),
	texSamLoc(-1),
	numTextures(0)
{
}

ShaderComponentMap::~ShaderComponentMap(){
}

std::string ShaderComponentMap::getVertexVariablesString(){
	return DEFINE + SHADER_COMPONENT_TEXTURE + ENDL;
}

std::string ShaderComponentMap::getFragmentVariablesString(){
	return
		DEFINE + SHADER_COMPONENT_TEXTURE + ENDL +
		"uniform sampler2D " + GL_UNIFORM_ID_TEXTURE_SAMPLER + "[" + std::to_string(MAX_TEXTURES) + "]" + SEMI_ENDL +
		"uniform int " + GL_UNIFORM_ID_NUM_TEXTURES + SEMI_ENDL;
}

std::string ShaderComponentMap::getVertexBodyString(){
	return "";
}

std::string ShaderComponentMap::getFragmentBodyString(){
	std::stringstream res;
	res << "vec4 textureRes;" << ENDL;
	res << "if (" + GL_UNIFORM_ID_NUM_TEXTURES + " > 0){" << ENDL;
		res << "\t\ttextureRes = vec4(texture(" << GL_UNIFORM_ID_TEXTURE_SAMPLER << "[0], " << GL_IN_OUT_FRAG_UV << ").rgba)" << SEMI_ENDL;
	res << "}" << SEMI_ENDL;
	res << "if (" + GL_UNIFORM_ID_NUM_TEXTURES + " > 1){" << ENDL;
		res << "\t\ttextureRes *= vec4(texture(" << GL_UNIFORM_ID_TEXTURE_SAMPLER << "[1], " << GL_IN_OUT_FRAG_UV << ").rgba)" << SEMI_ENDL;
	res << "}" << SEMI_ENDL;
	res << "modFrag *= textureRes" << SEMI_ENDL;
	if(alphaDiscardThreshold >= 0){
		res << "if(modFrag.a <= " << alphaDiscardThreshold << "){discard;}" << ENDL;
	}
	return res.str();
}

std::string ShaderComponentMap::getOutColorMod(){
	return GL_OUT_OUT_COLOR + " *= modFrag" + SEMI_ENDL;
}

void ShaderComponentMap::clean(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable){
	makeDirty();
	ShaderComponent::clean(_matrixStack, _renderOption, _nodeRenderable);
	//configureUniforms(_matrixStack, _renderOption, _nodeRenderable);
	//shader->configureUniforms(_matrixStack, _renderOption, _nodeRenderable);
}

void ShaderComponentMap::load(){
	if(!loaded){
		texNumLoc = glGetUniformLocation(shader->getProgramId(), GL_UNIFORM_ID_NUM_TEXTURES.c_str());
		texSamLoc = glGetUniformLocation(shader->getProgramId(), GL_UNIFORM_ID_TEXTURE_SAMPLER.c_str());
	}
	ShaderComponent::load();
}

void ShaderComponentMap::unload(){
	if(loaded){
		texNumLoc = -1;
		texSamLoc = -1;
		numTextures = 0;
	}
	ShaderComponent::unload();
}

void ShaderComponentMap::configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable){
	MeshInterface * mesh = dynamic_cast<MeshInterface *>(_nodeRenderable);
	if(mesh != nullptr){
		// check if the number of textures has changes and send new value to OpenGL
		unsigned long int newNumTextures = mesh->textureCount();
		if(newNumTextures != numTextures){
			glUniform1i(texNumLoc, newNumTextures);
			numTextures = newNumTextures;
		}
		// Bind each texture to the texture sampler array in the frag _shader
		for(unsigned long int i = 0; i < mesh->textureCount(); ++i){
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, mesh->getTexture(i)->textureId);
			glUniform1i(texSamLoc, i);
		}
	}else{
		glUniform1i(texNumLoc, numTextures); // should this be zero?
	}

	/*SpriteMesh * spriteMesh = dynamic_cast<SpriteMesh *>(_nodeRenderable);
	//Setup the texture for the current animation
	if(spriteMesh != nullptr){
	if(spriteMesh->animatedTexture != nullptr){
	glActiveTexture(GL_TEXTURE0 + 1 + numTextures);
	glBindTexture(GL_TEXTURE_2D, spriteMesh->animatedTexture->textureId);
	glUniform1i(texSamLoc, numTextures + 1);
	glUniform1i(texNumLoc, numTextures + 1);
	}
	}*/
}