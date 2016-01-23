#pragma once

#include <shader/ShaderComponentToon.h>

class PD_ShaderComponentSpecialToon : public ShaderComponentToon{
public:
	PD_ShaderComponentSpecialToon(ComponentShaderBase * _shader, Texture * _texture, bool _doubleSided);
	
	virtual std::string getFragmentBodyString() override;
};