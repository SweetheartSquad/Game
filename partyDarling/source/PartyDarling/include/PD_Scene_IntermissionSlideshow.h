#pragma once

#include <Scene_SlideShow.h>

class ComponentShaderText;
class TextLabel;

class PD_Scene_IntermissionSlideshow : public Scene_SlideShow{
private:
	TextLabel * skip;
	ComponentShaderText * textShader;
public:
	PD_Scene_IntermissionSlideshow(Game * _game, int _plotPosition = 1);
	~PD_Scene_IntermissionSlideshow();

	virtual void update(Step * _step) override;
};