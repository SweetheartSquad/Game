#pragma once

#include <PD_Scene_IntermissionSlideshow.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>

PD_Scene_IntermissionSlideshow::PD_Scene_IntermissionSlideshow(Game * _game, int _plotPosition) :
	Scene_SlideShow(_game)
{
	// get the slides
	Texture * tex = new Texture("assets/textures/introSlides/intermission_" + std::to_string(_plotPosition) + ".png", false, true);
	tex->load();
	push(new Slide(tex));

	// setup the trigger for moving on to the game after the slides are done
	eventManager->addEventListener("overflow", [_game](sweet::Event * _event){
		_game->scenes["game"] = new PD_Scene_Main(dynamic_cast<PD_Game*>(_game));
		_game->switchScene("game", true);
	});



	

	textShader = new ComponentShaderText(false);

	textShader->setColor(1.f, 1.f, 1.f);
	textShader->load();

	VerticalLinearLayout * vl = new VerticalLinearLayout(uiLayer->world);
	uiLayer->addChild(vl);
	vl->setRationalWidth(1.f, uiLayer);
	vl->setAutoresizeHeight();
	vl->verticalAlignment = kMIDDLE;
	vl->horizontalAlignment = kRIGHT;

	TextLabel * skip = new TextLabel(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-sub-font")->font, textShader);
	vl->addChild(skip);
	skip->setBackgroundColour(0.5f, 0, 0);
	skip->setRationalWidth(0.25f, vl);
	skip->setHeight(PD_ResourceManager::scenario->getFont("options-menu-sub-font")->font->getLineHeight()*2.f);
	skip->verticalAlignment = kMIDDLE;
	skip->horizontalAlignment = kCENTER;
	skip->setText("continue");
	skip->eventManager->addEventListener("click", [this](sweet::Event * _event){
		eventManager->triggerEvent("overflow");
	});
	skip->setVisible(false);

	Timeout * t = new Timeout(1.f, [skip](sweet::Event * _event){
		skip->setVisible(true);
		skip->setMouseEnabled(true);
	});
	t->start();
	childTransform->addChild(t, false);



	uiLayer->invalidateLayout();

	// advance to the first slide
	changeSlide(true);
}

PD_Scene_IntermissionSlideshow::~PD_Scene_IntermissionSlideshow(){
	delete textShader;
}

void PD_Scene_IntermissionSlideshow::update(Step * _step){
	Scene_SlideShow::update(_step);
}