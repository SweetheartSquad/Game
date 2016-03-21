#pragma once

#include <Scene_SlideShow.h>

class PD_Scene_EndingSlideshow : public Scene{
private:
	unsigned long int curSlide;
	std::vector<Texture *> slides;
	UILayer * uiLayer;
	NodeUI * slideDisplay;
public:
	PD_Scene_EndingSlideshow(Game * _game);
	~PD_Scene_EndingSlideshow();

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
};