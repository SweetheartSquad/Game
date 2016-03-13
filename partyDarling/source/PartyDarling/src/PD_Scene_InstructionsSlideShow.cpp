#pragma once

#include <PD_Scene_InstructionsSlideShow.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>

PD_Scene_InstructionsSlideShow::PD_Scene_InstructionsSlideShow(Game * _game) :
	Scene_SlideShow(_game)
{
	// get the slides
	Texture * tex;
	tex = new Texture("assets/textures/introSlides/6.png", false, true);
	tex->load();
	push(new Slide(tex));
	tex = new Texture("assets/textures/introSlides/7.png", false, true);
	tex->load();
	push(new Slide(tex));
	for(unsigned long int i = 1; i <= 7; ++i){
		tex = new Texture("assets/textures/introSlides/tut-0"+std::to_string(i)+".png", false, true);
		tex->load();
		push(new Slide(tex));
	}

	// setup the trigger for moving back to the menu when the slides are done
	auto f = [_game](sweet::Event * _event){
		_game->switchScene("options", true);
	};
	eventManager->addEventListener("overflow", f);
	eventManager->addEventListener("underflow", f);

	uiLayer->invalidateLayout();

	// advance to the first slide
	changeSlide(true);
}

PD_Scene_InstructionsSlideShow::~PD_Scene_InstructionsSlideShow(){
}

void PD_Scene_InstructionsSlideShow::update(Step * _step){
	if(mouse->leftJustPressed()){
		changeSlide(true);
	}if(mouse->rightJustPressed()){
		changeSlide(false);
	}
	Scene_SlideShow::update(_step);
}