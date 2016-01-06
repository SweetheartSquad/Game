#pragma once

#include <PD_UI_Bubble.h>
#include <PD_ResourceManager.h>

PD_UI_Bubble::PD_UI_Bubble(BulletWorld * _world) :
	NodeUI(_world),
	currentOption(0),
	vl(new VerticalLinearLayout(world))
{
	textShader = new ComponentShaderText(true);
	textShader->setColor(0,0,0);
	addChild(vl);
	setRationalHeight(1.f);
	setRationalWidth(1.f);
	
	vl->setRationalHeight(1.f);
	vl->setRationalWidth(1.f);

	vl->horizontalAlignment = kCENTER;
	vl->verticalAlignment = kMIDDLE;

	background->setVisible(false);

	test = new Transform();
	childTransform->addChild(test, false);
}

void PD_UI_Bubble::addOption(std::string _text){
	Texture_NineSliced * tex = new Texture_NineSliced("assets/textures/nineslicing.png", true);
	tex->load();
	NodeUI_NineSliced * optionBubble = new NodeUI_NineSliced(world, tex);
	TextLabel * optionText = new TextLabel(world, PD_ResourceManager::scenario->defaultFont->font, textShader);
	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	//vl->addChild(optionText);
	//optionText->setRationalWidth(0.8f, vl);
	optionText->setMouseEnabled(true);
	optionText->background->setColour(1,1,1,1);
	optionText->background->setVisible(true);
	optionText->setText(_text);
	optionText->eventManager.addEventListener("click", [this, _text](sweet::Event * _event){
		std::cout << "clicked " << _text << std::endl;
	});
	options.push_back(optionBubble);
	
	optionBubble->setBorder(25.f);
	optionBubble->setWidth(250);
	optionBubble->setHeight(100);
	//optionBubble->setMargin(5);
	optionBubble->invalidateLayout();
	test->addChild(optionBubble);
	optionBubble->addChild(vl);
	vl->addChild(optionText);
	vl->setRationalHeight(1.f, optionBubble);
	vl->setRationalWidth(1.f, optionBubble);
	vl->horizontalAlignment = kCENTER;
	vl->verticalAlignment = kMIDDLE;
	optionText->horizontalAlignment = kCENTER;
	optionText->setRationalWidth(1.f, vl);
	//invalidateLayout();
}

void PD_UI_Bubble::select(unsigned long int _option){

}

void PD_UI_Bubble::selectCurrent(){

}

void PD_UI_Bubble::update(Step * _step){
	NodeUI::update(_step);

	float w = getWidth(true, true);
	float h = getHeight(true, true);
	test->translate(w/2, h/2, 0, false);

	for(unsigned long int i = 0; i < options.size(); ++i){
		float offset = (i+currentOption) % options.size();
		offset -= options.size()/2.f;
		float spacing = 100;
		options.at(i)->firstParent()->translate(0, offset*spacing, i == currentOption ? 0 : -1, false);
	}
}