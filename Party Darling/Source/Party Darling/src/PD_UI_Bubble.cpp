#pragma once

#include <PD_UI_Bubble.h>
#include <PD_ResourceManager.h>

Bubble::Bubble(BulletWorld * _world, Texture_NineSliced * _tex, Shader * _textShader) :
	NodeUI_NineSliced(_world, _tex)
{
	label = new TextLabel(world, PD_ResourceManager::scenario->defaultFont->font, _textShader);
	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	setBorder(15.f);
	setWidth(350);
	setHeight(75);
	invalidateLayout();

	addChild(vl);
	vl->addChild(label);
	vl->setRationalHeight(1.f, this);
	vl->setRationalWidth(1.f, this);
	vl->horizontalAlignment = kCENTER;
	vl->verticalAlignment = kMIDDLE;
	label->horizontalAlignment = kCENTER;
	label->setRationalWidth(1.f, vl);
}

PD_UI_Bubble::PD_UI_Bubble(BulletWorld * _world) :
	NodeUI(_world),
	currentOption(0),
	vl(new VerticalLinearLayout(world)),
	displayOffset(0)
{
	textShader = new ComponentShaderText(true);
	textShader->setColor(0,0,0);
	++textShader->referenceCount;
	addChild(vl);
	setRationalHeight(0.25f);
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
	while(unusedOptions.size() > 0){
		delete unusedOptions.top()->firstParent();
		unusedOptions.pop();
	}
	deleteChildTransform();
	delete bubbleTex;
	textShader->decrementAndDelete();
}

void PD_UI_Bubble::addOption(std::string _text){
	Bubble * optionBubble;
	if(unusedOptions.size() > 0){
		optionBubble = unusedOptions.top();
		unusedOptions.pop();
	}else{
		optionBubble = new Bubble(world, bubbleTex, textShader);
		Transform * t = new Transform();
		t->addChild(optionBubble, false);
	}
	
	optionBubble->label->setText(_text);
	options.push_back(optionBubble);
	
	test->addChild(optionBubble->firstParent(), false);
}

void PD_UI_Bubble::select(unsigned long int _option){

}

void PD_UI_Bubble::selectCurrent(){

}

void PD_UI_Bubble::placeOptions(){
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
}

void PD_UI_Bubble::update(Step * _step){
	{
	float w = getWidth(true, true);
	float h = getHeight(true, true);
	test->translate(w/2, h/2, 0, false);
	}
	
	float targetDispayOffset = options.size() > 0 ? ((float)currentOption / options.size() + 0.1f) : 0;
	float delta = targetDispayOffset - displayOffset;
	if(std::abs(delta) > FLT_EPSILON){
		displayOffset += (targetDispayOffset - displayOffset) * 0.2f;
		placeOptions();
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

void PD_UI_Bubble::clear(){
	while(options.size() > 0){
		test->removeChild(options.back()->firstParent());
		//delete options.back()->firstParent();
		unusedOptions.push(options.back());
		options.pop_back();
	}
}