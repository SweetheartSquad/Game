#pragma once

#include <PD_Scene_IntroSlideShow.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>

PD_Scene_IntroSlideShow::PD_Scene_IntroSlideShow(Game * _game) :
	Scene_SlideShow(_game)
{
	// get the slides
	for(unsigned long int i = 1; i <= 3; ++i){
		Texture * tex = new Texture("assets/textures/introSlides/" + std::to_string(i) + ".png", false, true);
		tex->load();
		push(new Slide(tex));
	}

	// setup the trigger for moving on to the game after the slides are done
	eventManager->addEventListener("overflow", [_game](sweet::Event * _event){
		dynamic_cast<PD_Game *>(_game)->showLoading("Loading...", 0);
		_game->scenes["game"] = new PD_Scene_Main(dynamic_cast<PD_Game*>(_game));
		_game->switchScene("game", true);
	});

	// advance to the first slide
	changeSlide(true);
}

void PD_Scene_IntroSlideShow::update(Step * _step){
	if(mouse->leftJustPressed()){
		changeSlide(true);
	}if(mouse->rightJustPressed()){
		changeSlide(false);
	}
	Scene_SlideShow::update(_step);
}