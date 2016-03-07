#pragma once

#include <PD_UI_DissCard.h>
#include <PD_ResourceManager.h>

#include <Player.h>
#include <PD_Character.h>
#include <PD_DissStats.h>

PD_UI_DissCard::PD_UI_DissCard(BulletWorld * _world, Player * _player) :
	NodeUI_NineSliced(_world, dynamic_cast<Texture_NineSliced *>(PD_ResourceManager::scenario->getTexture("DISSCARD-BUBBLE")->texture)),
	dissStats(_player->dissStats),
	showSlider(true),
	name("Player"),
	experience(&_player->experience) // TODO: tie this to player experience
{
	init();
	setLevel(_player->level);
}
PD_UI_DissCard::PD_UI_DissCard(BulletWorld * _world) :
	NodeUI_NineSliced(_world, dynamic_cast<Texture_NineSliced *>(PD_ResourceManager::scenario->getTexture("DISSCARD-BUBBLE")->texture)),
	dissStats(nullptr),
	showSlider(false),
	name(""),
	experience(nullptr),
	slider(nullptr),
	level(nullptr)
{
	init();
}

PD_UI_DissCard::~PD_UI_DissCard(){
}

void PD_UI_DissCard::init(){
	textShader = new ComponentShaderText(true);
	textShader->setColor(113/255.f, 71/255.f, 16/255.f);

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
	updateStats();

	// exp slider
	if(showSlider){
		HorizontalLinearLayout * xpContainer = new HorizontalLinearLayout(world);
		layout->addChild(xpContainer);
		xpContainer->setRationalWidth(1.f, layout);
		xpContainer->setRationalHeight(0.1f, layout);
		xpContainer->horizontalAlignment = kCENTER;
		xpContainer->verticalAlignment = kMIDDLE;

		slider = new SliderControlled(world, experience, 0.f, 100.f);
		//slider->boxSizing = kCONTENT_BOX;
		//slider->marginTop.setRationalSize(0.05f, &layout->height);
		xpContainer->addChild(slider);
		slider->setRationalWidth(0.75f, xpContainer);
		slider->setRationalHeight(0.5f, xpContainer);

		slider->thumb->setVisible(false);
	
		slider->setBackgroundColour(1,1,1,1);
		slider->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-TRACK")->texture);
		slider->background->mesh->setScaleMode(GL_NEAREST);
	
		slider->fill->setBackgroundColour(1,1,1,1);
		slider->fill->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("DISSCARD-FILL")->texture);
		slider->fill->background->mesh->setScaleMode(GL_NEAREST);

		level = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
		xpContainer->addChild(level);
		level->setRationalWidth(0.25, xpContainer);
		level->setRationalHeight(1.f, xpContainer);
		level->horizontalAlignment = kCENTER;

	}else{
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

void PD_UI_DissCard::updateStats(){
	if(dissStats != nullptr){
		// show earned stars and hide unearned stars
		for(unsigned long int x = 0; x < 5; ++x){
			stars[0][x]->setVisible(x < dissStats->getDefense());
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[1][x]->setVisible(x < dissStats->getInsight());
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[2][x]->setVisible(x < dissStats->getStrength());
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[3][x]->setVisible(x < dissStats->getSass());
		}
	}else{
		for(unsigned long int x = 0; x < 5; ++x){
			stars[0][x]->setVisible(false);
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[1][x]->setVisible(false);
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[2][x]->setVisible(false);
		}for(unsigned long int x = 0; x < 5; ++x){
			stars[3][x]->setVisible(false);
		}
	}
}

void PD_UI_DissCard::setLevel(int _level){
	if(level != nullptr){
		std::stringstream s;
		s << "Lvl. " << _level;
		level->setText(s.str());
	}
}

void PD_UI_DissCard::setEnemy(PD_Character * _enemy){
	dissStats = _enemy->dissStats;

	updateStats();

	label->setText(_enemy->definition->name);
}