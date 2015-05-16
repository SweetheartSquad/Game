#pragma once

#include <NodeUI.h>
#include <Label.h>

class PD_Button : public NodeUI{
public:

	Label * normalLabel;
	Label * downLabel;
	Label * overLabel;

	PD_Button(BulletWorld * _world, Scene * _scene);
	
	void renderDown(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	void renderOver(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	void renderDefault(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};