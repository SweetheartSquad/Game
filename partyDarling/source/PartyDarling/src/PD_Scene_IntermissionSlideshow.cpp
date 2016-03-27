#pragma once

#include <PD_Scene_IntermissionSlideshow.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>

PD_Scene_IntermissionSlideshow::PD_Scene_IntermissionSlideshow(Game * _game, int _plotPosition) :
	Scene_SlideShow(_game)
{
	push(new Slide(PD_ResourceManager::scenario->getTexture("INTERMISSION_"+std::to_string(_plotPosition))->texture));
	
	slideNew->background->mesh->setScaleMode(GL_NEAREST);
	slideOld->background->mesh->setScaleMode(GL_NEAREST);

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

	skip = new TextLabel(uiLayer->world, PD_ResourceManager::scenario->getFont("options-menu-sub-font")->font, textShader);
	vl->addChild(skip);
	//skip->setBackgroundColour(0.5f, 0, 0);
	skip->setRationalWidth(1.f, vl);
	skip->setRationalHeight(0.5f, uiLayer);
	skip->verticalAlignment = kMIDDLE;
	skip->horizontalAlignment = kCENTER;
	skip->setText("Click anywhere to Continue...");
	skip->eventManager->addEventListener("click", [this](sweet::Event * _event){
		eventManager->triggerEvent("overflow");
	});
	skip->setVisible(false);

	Timeout * t = new Timeout(1.f, [this](sweet::Event * _event){
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

	if(skip->isVisible()){
		if(mouse->leftJustPressed()){
			eventManager->triggerEvent("overflow");
		}
	}
}