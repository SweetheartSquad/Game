#pragma once

#include <PD_Scene_EndingSlideshow.h>
#include <PD_Scene_MenuMain.h>
#include <PD_ResourceManager.h>

#include <PD_Scene_Main.h>
#include <RenderOptions.h>

PD_Scene_EndingSlideshow::PD_Scene_EndingSlideshow(Game * _game) :
	Scene(_game),
	uiLayer(new UILayer(0,0,0,0)),
	curSlide(0)
{
	unsigned long int numSlides = 3;
	for(unsigned long int i = 1; i <= numSlides; ++i){
		slides.push_back(new Texture("assets/textures/introSlides/ending_" + std::to_string(i) + ".png", false, false));
		slides.back()->load();
	}


	slideDisplay = new NodeUI(uiLayer->world);
	slideDisplay->setRationalHeight(1.f, uiLayer);
	slideDisplay->setRationalWidth(1.f, uiLayer);
	slideDisplay->background->mesh->setScaleMode(GL_NEAREST);
	slideDisplay->setBackgroundColour(1,1,1, 0);
	uiLayer->addChild(slideDisplay);

	uiLayer->invalidateLayout();



	Timeout * t = new Timeout(2.f, [this, numSlides, t](sweet::Event * _event){
		slideDisplay->setBackgroundColour(1,1,1, 0);
		if(curSlide >= numSlides){
			dynamic_cast<PD_Scene_MenuMain *>(game->scenes["menu"])->fadeTimeout->restart();	
			dynamic_cast<PD_Scene_MenuMain *>(game->scenes["menu"])->fadeNode->setBackgroundColour(0,0,0,1);
			game->switchScene("menu", true);
		}else{
			slideDisplay->background->mesh->replaceTextures(slides.at(curSlide));
			t->restart();
			++curSlide;
		}
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		if(p < 0.25f){
			p = sweet::NumberUtils::map(p, 0.f, 0.25f, 0.f, 1.f);
		}else if(p > 0.75f){
			p = sweet::NumberUtils::map(p, 0.75f, 1.f, 1.f, 0.f);
		}else{
			p = 1.f;
		}
		slideDisplay->setBackgroundColour(1,1,1, p);
	});

	t->trigger();
	t->start();
	childTransform->addChild(t, false);

}

PD_Scene_EndingSlideshow::~PD_Scene_EndingSlideshow(){
	slideDisplay->background->mesh->clearTextures();
	delete uiLayer;
	while(slides.size() > 0){
		delete slides.back();
		slides.pop_back();
	}
}

void PD_Scene_EndingSlideshow::update(Step * _step){
	Scene::update(_step);
	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer->resize(0,sd.x,0,sd.y);
	uiLayer->update(_step);
}
void PD_Scene_EndingSlideshow::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->setClearColour(0,0,0,0);
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer->render(_matrixStack, _renderOptions);
}