#pragma once

#include <shader/ShaderComponent.h>

class ShaderComponentVNoise : public ShaderComponent{
public:
	GLint timeLocation;
	GLint mag1Location;
	GLint mag2Location;

	float mag1, mag2;

	ShaderComponentVNoise(ComponentShaderBase * _shader);
	~ShaderComponentVNoise();
	
	virtual std::string getVertexVariablesString() override;
	virtual std::string getFragmentVariablesString() override;
	virtual std::string getVertexBodyString() override;
	virtual std::string getFragmentBodyString() override;
	virtual std::string getOutColorMod() override;
	void load() override;
	void unload() override;
	virtual void configureUniforms(sweet::MatrixStack* _matrixStack, RenderOptions* _renderOption, NodeRenderable* _nodeRenderable) override;

	void setMag(float _mag1, float _mag2, float _interpolation);
};