#pragma once

#include <PD_UI_ItemGainLoss.h>
#include <PD_ResourceManager.h>

#include <PD_Item.h>
#include <Timeout.h>
#include <Easing.h>

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
	container->setRationalHeight(0.3f, this);
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

	animationTimeout = new Timeout(2.f, [this](sweet::Event * _event){
		setVisible(false);
	});

	animationTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		float a;

		if(p  <= 0.5f){
			a = Easing::easeOutCubic(p, 0.f, 1.f, 0.5f);
		}else if(p < 0.9f){
			a = 1.f;
		}else{
			a = Easing::easeInCubic(p - 0.9f, 1.f, -1.f, 0.1f);
		}

		image->setBackgroundColour(1.f, 1.f, 1.f, a);
		textShader->setColor(1.f, 1.f, 1.f, a);
	});
	childTransform->addChild(animationTimeout, false);

}

PD_UI_ItemGainLoss::~PD_UI_ItemGainLoss(){

}

void PD_UI_ItemGainLoss::gainItem(PD_Item * _item){
	setVisible(true);
	AssetTexture * tex = PD_ResourceManager::itemTextures->getTexture(_item->definition->texture);
	tex->load();
	image->background->mesh->replaceTextures(tex->texture);
	
	text->setText("Acquired " + _item->definition->name);

	image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
	textShader->setColor(1.f, 1.f, 1.f, 0.f);
	animationTimeout->restart();
}

void PD_UI_ItemGainLoss::loseItem(PD_Item * _item){
	setVisible(true);
	AssetTexture * tex = PD_ResourceManager::itemTextures->getTexture(_item->definition->texture);
	tex->load();
	image->background->mesh->replaceTextures(tex->texture);

	text->setText("Gave " + _item->definition->name);

	image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
	textShader->setColor(1.f, 1.f, 1.f, 0.f);
	animationTimeout->restart();
}