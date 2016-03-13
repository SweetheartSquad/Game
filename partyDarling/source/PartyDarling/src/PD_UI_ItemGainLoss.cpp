#pragma once

#include <PD_UI_ItemGainLoss.h>
#include <PD_ResourceManager.h>

#include <PD_Item.h>
#include <Timeout.h>
#include <Easing.h>

#define UI_MESSAGE_DURATION 2.f

PD_UI_ItemGainLoss::PD_UI_ItemGainLoss(BulletWorld * _world) :
	VerticalLinearLayout(_world),
	textShader(new ComponentShaderText(false))
{
	setVisible(false);
	// Pickup/Remove Item Animaiton
	horizontalAlignment = kCENTER;
	verticalAlignment = kMIDDLE;

	HorizontalLinearLayout * container = new HorizontalLinearLayout(world);
	addChild(container);
	container->horizontalAlignment = kCENTER;
	container->verticalAlignment = kMIDDLE;
	container->setRationalHeight(0.2f, this);
	container->setSquareWidth(1.f);

	image = new NodeUI(world);
	container->addChild(image);
	image->setRationalHeight(1.f, container);
	image->setSquareWidth(1.f);
	image->background->mesh->setScaleMode(GL_NEAREST);

	text = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
	addChild(text);
	text->setRationalHeight(0.1f, this);
	text->setRationalWidth(1.f, this);
	text->horizontalAlignment = kCENTER;

}

PD_UI_ItemGainLoss::~PD_UI_ItemGainLoss(){
	for(auto t : animationTimeouts){
		t->stop();
		delete t;
	}
	animationTimeouts.clear();
}

void PD_UI_ItemGainLoss::gainItem(PD_Item * _item){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
	});
	t->eventManager->addEventListener("start", [this, _item](sweet::Event * _event){
		setVisible(true);

		AssetTexture * tex = PD_ResourceManager::itemTextures->getTexture(_item->definition->texture);
		tex->load();
		image->background->mesh->replaceTextures(tex->texture);
	
		text->setText("Acquired " + _item->definition->name);

		image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
		textShader->setColor(1.f, 1.f, 1.f, 0.f);
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		animate(p);
	});

	animationTimeouts.push_back(t);
}

void PD_UI_ItemGainLoss::loseItem(PD_Item * _item){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
	});
	t->eventManager->addEventListener("start", [this, _item](sweet::Event * _event){
		setVisible(true);

		AssetTexture * tex = PD_ResourceManager::itemTextures->getTexture(_item->definition->texture);
		tex->load();
		image->background->mesh->replaceTextures(tex->texture);

		text->setText("Gave " + _item->definition->name);

		image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
		textShader->setColor(1.f, 1.f, 1.f, 0.f);
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		animate(p);
	});

	animationTimeouts.push_back(t);
}

void PD_UI_ItemGainLoss::displayMessage(std::string _message){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
		image->setVisible(true);
	});
	t->eventManager->addEventListener("start", [this, _message](sweet::Event * _event){
		setVisible(true);
		image->setVisible(false);
		text->setText(_message);
		textShader->setColor(1.f, 1.f, 1.f, 0.f);
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		animate(p);
	});

	animationTimeouts.push_back(t);
}

void PD_UI_ItemGainLoss::animate(float _p){
	float a;
	if(_p  <= 0.5f){
		a = Easing::easeOutCubic(_p, 0.f, 1.f, 0.5f);
	}else if(_p < 0.8f){
		a = 1.f;
	}else{
		a = Easing::easeInCubic(_p - 0.8f, 1.f, -1.f, 0.2f);
	}

	image->setBackgroundColour(1.f, 1.f, 1.f, a);
	textShader->setColor(1.f, 1.f, 1.f, a);
}

void PD_UI_ItemGainLoss::update(Step * _step){
	if(animationTimeouts.size() > 0){
		Timeout * t = animationTimeouts.front();
		if(!t->active){
			t->start();
		}

		t->update(_step);
		if(t->complete){
			delete t;
			animationTimeouts.erase(animationTimeouts.begin());
		}
	}
	VerticalLinearLayout::update(_step);
}