#pragma once

#include <Scene_SlideShow.h>

class ComponentShaderText;
class Timeout;

class PD_Scene_IntroSlideShow : public Scene_SlideShow{
private:
	ComponentShaderText * textShader;
	Timeout * changeSlideTimeout;
	bool isForwads;
public:
	PD_Scene_IntroSlideShow(Game * _game);
	~PD_Scene_IntroSlideShow();

	virtual void update(Step * _step) override;
};