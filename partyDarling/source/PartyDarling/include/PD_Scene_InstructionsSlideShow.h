#pragma once

#include <Scene_SlideShow.h>

class ComponentShaderText;

class PD_Scene_InstructionsSlideShow : public Scene_SlideShow{
public:
	PD_Scene_InstructionsSlideShow(Game * _game);
	~PD_Scene_InstructionsSlideShow();

	virtual void update(Step * _step) override;
};