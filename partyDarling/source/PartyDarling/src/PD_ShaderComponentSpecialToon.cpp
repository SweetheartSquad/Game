#pragma once

#include <PD_ShaderComponentSpecialToon.h>
#include <sstream>

#include <shader/ShaderVariables.h>

PD_ShaderComponentSpecialToon::PD_ShaderComponentSpecialToon(ComponentShaderBase * _shader, Texture * _texture, bool _doubleSided) :
	ShaderComponentToon(_shader, _texture, _doubleSided)
{
}

std::string PD_ShaderComponentSpecialToon::getFragmentBodyString(){
	std::stringstream res;
	res << ShaderComponentToon::getFragmentBodyString() <<
	"if(level == " << GL_UNIFORM_ID_TOON_LEVELS << "-1){toonMod *= luminance;}" << ENDL;
	return res.str();
}