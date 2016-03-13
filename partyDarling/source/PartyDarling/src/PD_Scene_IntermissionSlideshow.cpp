#pragma once

#include <PD_Scene_IntermissionSlideshow.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>

PD_Scene_IntermissionSlideshow::PD_Scene_IntermissionSlideshow(Game * _game, int _plotPosition) :
	Scene_SlideShow(_game)
{
	// get the slides
	Texture * tex;
	std::stringstream s;
	s << "assets/textures/introSlides/intermission_" << _plotPosition << ".png";
	tex = new Texture(s.str(), false, true);
	tex->load();
	push(new Slide(tex));

	// setup the trigger for moving on to the game after the slides are done
	eventManager->addEventListener("overflow", [_game](sweet::Event * _event){
		_game->scenes["game"] = new PD_Scene_Main(dynamic_cast<PD_Game*>(_game));
		_game->switchScene("game", true);
	});

	uiLayer->invalidateLayout();

	// advance to the first slide
	changeSlide(true);
}

PD_Scene_IntermissionSlideshow::~PD_Scene_IntermissionSlideshow(){
}

void PD_Scene_IntermissionSlideshow::update(Step * _step){
	if(mouse->leftJustPressed()){
		changeSlide(true);
	}if(mouse->rightJustPressed()){
		changeSlide(false);
	}
	Scene_SlideShow::update(_step);
}