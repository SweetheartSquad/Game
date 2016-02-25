#pragma once

#include <Scene_SlideShow.h>

class PD_Scene_IntroSlideShow : public Scene_SlideShow{
public:
	PD_Scene_IntroSlideShow(Game * _game);

	virtual void update(Step * _step) override;
};