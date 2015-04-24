#pragma once

#include <Box2DMeshEntity.h>

class Material;

class PD_Player : public Box2DMeshEntity{
public:
	
	Material * playerMat;

	PD_Player(Box2DWorld * _world);

	void update(Step * _step);
	void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	void setShader(Shader * _shader, bool _def) override;
};