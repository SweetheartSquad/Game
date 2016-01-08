#pragma once

#include <PD_UI_Bubble.h>
#include <PD_ResourceManager.h>

PD_UI_Bubble::PD_UI_Bubble(BulletWorld * _world) :
	NodeUI(_world),
	currentOption(0),
	vl(new VerticalLinearLayout(world)),
	displayOffset(0)
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

	bubbleTex = new Texture_NineSliced("assets/textures/nineslicing.png", true);
	bubbleTex->load();
}

PD_UI_Bubble::~PD_UI_Bubble(){
	deleteChildTransform();
	delete bubbleTex;
}

void PD_UI_Bubble::addOption(std::string _text){
	NodeUI_NineSliced * optionBubble = new NodeUI_NineSliced(world, bubbleTex);
	
	//NodeUI * optionBubble = new NodeUI(world);
	
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
	
	optionBubble->setBorder(15.f);
	optionBubble->setWidth(350);
	optionBubble->setHeight(75);
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
	{
	float w = getWidth(true, true);
	float h = getHeight(true, true);
	test->translate(w/2, h/2, 0, false);
	}

	displayOffset += (((float)currentOption / options.size() + 0.1f) - displayOffset) * 0.2f;
	
	float verticalSpacing = 50.f;
	float bubbleWidth = 200;
	for(unsigned long int i = 0; i < options.size(); ++i){
		float offset = (float)i / options.size();
		offset -= displayOffset;
		offset *= glm::pi<float>()*2.f;
		float w = ((glm::cos(offset) + 1)*0.5f + 1.f)*bubbleWidth;

		options.at(i)->firstParent()->translate(-w/2.f, glm::sin(offset)*verticalSpacing, 0, false);
		options.at(i)->setWidth(w);
		//options.at(i)->setBorder(std::max(2.f,w/30.f));
		options.at(i)->setBackgroundColour(1,1,1, i == currentOption ? 1.f : 0.5f);
		options.at(i)->invalidateLayout();
	}


	

	

	NodeUI::update(_step);
}

void PD_UI_Bubble::next(){
	if(++currentOption >= options.size()){
		currentOption = 0;
		displayOffset -= 1;
	}
	reorderChildren();
}

void PD_UI_Bubble::prev(){
	if(--currentOption >= options.size()){
		currentOption = options.size()-1;
		displayOffset += 1;
	}
	reorderChildren();
}

void PD_UI_Bubble::reorderChildren(){
	unsigned long int currentOptionActual = currentOption;
	std::vector<NodeUI_NineSliced *>optionsOrdered;
	while(optionsOrdered.size() < options.size()){
		if(++currentOptionActual >= options.size()){
			currentOptionActual = 0;
		}
		optionsOrdered.push_back(options.at(currentOptionActual));
		test->removeChild(options.at(currentOptionActual)->firstParent());
	}

	for(auto o : optionsOrdered){
		test->addChild(o->firstParent(), false);
	}
}