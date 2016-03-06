#pragma once

#include <PD_UI_DissCard.h>
#include <PD_ResourceManager.h>

#include <Player.h>
#include <PD_Character.h>

PD_UI_DissCard::PD_UI_DissCard(BulletWorld * _world, Player * _player) :
	NodeUI_NineSliced(_world, dynamic_cast<Texture_NineSliced *>(PD_ResourceManager::scenario->getTexture("DISSCARD-BUBBLE")->texture)),
	defense(_player->defense),
	insight(_player->insight),
	strength(_player->strength),
	sass(_player->sass),
	showSlider(true),
	name("Player"),
	experience(new float(0.5f)) // TODO: tie this to player experience
{
	init();
}
PD_UI_DissCard::PD_UI_DissCard(BulletWorld * _world, PD_Character * _enemy) :
	NodeUI_NineSliced(_world, dynamic_cast<Texture_NineSliced *>(PD_ResourceManager::scenario->getTexture("DISSCARD-BUBBLE")->texture)),
	defense(_enemy->defense),
	insight(_enemy->insight),
	strength(_enemy->strength),
	sass(_enemy->sass),
	showSlider(false),
	name(_enemy->definition->name),
	experience(nullptr)
{
	init();
}

PD_UI_DissCard::~PD_UI_DissCard(){
}

void PD_UI_DissCard::init(){

	setBorder(25);
	setScaleMode(GL_NEAREST);
	VerticalLinearLayout * layout = new VerticalLinearLayout(world);
	c->addChild(layout);
	layout->setRationalHeight(1.f, c);
	layout->setSquareWidth(7.f/5.f);
	layout->setBackgroundColour(1,1,1,1);
	layout->background->setVisible(true);
	layout->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-BG")->texture);
	layout->background->mesh->setScaleMode(GL_NEAREST);

	for(unsigned long int y = 0; y < 4; ++y){
		HorizontalLinearLayout * row = new HorizontalLinearLayout(world);
		layout->addChild(row);
		row->setRationalWidth(0.75f, layout);
		row->setSquareHeight(1.f/5.f);

		for(unsigned long int x = 0; x < 5; ++x){
			NodeUI * star = new NodeUI(world);
			row->addChild(star);
			star->setRationalHeight(1.f, row);
			star->setSquareWidth(1.f);
			star->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-STAR")->texture);
			star->background->mesh->setScaleMode(GL_NEAREST);

			stars[y][x] = star;
		}
	}
	// show earned stars and hide unearned stars
	for(unsigned long int x = 0; x < 5; ++x){
		stars[0][x]->setVisible(x < defense);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[1][x]->setVisible(x < insight);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[2][x]->setVisible(x < strength);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[3][x]->setVisible(x < sass);
	}

	// exp slider
	if(showSlider){
		SliderController * slider = new SliderController(world, experience, 0.5f, 0.f, 1.f);
		slider->boxSizing = kCONTENT_BOX;
		slider->marginTop.setRationalSize(0.05f, &layout->height);
		layout->addChild(slider);
		slider->setRationalWidth(0.75f, layout);
		slider->setSquareHeight(1.f/10.f);

		slider->thumb->setVisible(false);
	
		slider->setBackgroundColour(1,1,1,1);
		slider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-TRACK")->texture);
		slider->background->mesh->setScaleMode(GL_NEAREST);
	
		slider->fill->setBackgroundColour(1,1,1,1);
		slider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-FILL")->texture);
		slider->fill->background->mesh->setScaleMode(GL_NEAREST);
	}else{
		textShader = new ComponentShaderText(true);
		textShader->setColor(113/255.f, 71/255.f, 16/255.f);
		label = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
		layout->addChild(label);
		label->boxSizing = kCONTENT_BOX;
		label->marginTop.setRationalSize(0.05f, &layout->height);
		label->setRationalWidth(0.75f, layout);
		label->setSquareHeight(1.f/10.f);
		label->setText(name);
	}

	invalidateLayout();
}

void PD_UI_DissCard::setEnemy(PD_Character * _enemy){
	defense = _enemy->defense;
	insight = _enemy->insight;
	strength = _enemy->strength;
	sass = _enemy->sass;

	// show earned stars and hide unearned stars
	for(unsigned long int x = 0; x < 5; ++x){
		stars[0][x]->setVisible(x < defense);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[1][x]->setVisible(x < insight);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[2][x]->setVisible(x < strength);
	}for(unsigned long int x = 0; x < 5; ++x){
		stars[3][x]->setVisible(x < sass);
	}

	label->setText(_enemy->definition->name);
}