#pragma once

#include <PD_UI_Bubble.h>
#include <PD_ResourceManager.h>
#include <Mouse.h>

Bubble::Bubble(BulletWorld * _world, Texture_NineSliced * _tex, Shader * _textShader) :
	NodeUI_NineSliced(_world, _tex)
{
	label = new TextLabel(world, PD_ResourceManager::scenario->getFont("FONT")->font, _textShader);
	VerticalLinearLayout * vl = new VerticalLinearLayout(world);
	setBorder(15.f);
	setWidth(1000);
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
	displayOffset(0),
	childrenUpdated(false)
{
	textShader = new ComponentShaderText(true);
	textShader->setColor(0,0,0);
	++textShader->referenceCount;
	addChild(vl);
	//setRationalHeight(0.25f);
	//setRationalWidth(1.f);
	
	vl->setRationalHeight(1.f, this);
	vl->setRationalWidth(1.f, this);

	vl->horizontalAlignment = kCENTER;
	vl->verticalAlignment = kMIDDLE;

	background->setVisible(false);

	test = new Transform();
	childTransform->addChild(test, false);

	bubbleTex = PD_ResourceManager::scenario->getNineSlicedTexture("NINESLICED");
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

void PD_UI_Bubble::addOption(std::string _text, sweet::EventManager::Listener _listener){
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
	
	optionBubble->eventManager.addEventListener("selected", _listener);

	test->addChild(optionBubble->firstParent(), false);
	childrenUpdated = true;
}

void PD_UI_Bubble::select(unsigned long int _option){
	options.at(_option)->eventManager.triggerEvent("selected");
}

void PD_UI_Bubble::selectCurrent(){
	select(currentOption);
}

void PD_UI_Bubble::placeOptions(){
	float verticalSpacing = 50.f;
	float bubbleWidth = 500;
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
	// don't bother with interaction and layout stuff if it's hidden or there aren't any options right now
	if(isVisible() && options.size() > 0){
		// use the mouse to determine interactions
		// if we're scrolling, then we either call "next" or "prev" based on the direction of the scroll
		// if we're not scrolling, we can click the current option
		Mouse & mouse = Mouse::getInstance();
		float d = mouse.getMouseWheelDelta();
		// if there's only one option, override and say we aren't scrolling
		if(options.size() < 2){
			d = 0;
		}

		// interaction 
		if(enabled){
			if(d > FLT_EPSILON){
				next();
			}else if(d < -FLT_EPSILON){
				prev();
			}else if(mouse.leftJustPressed()){
				selectCurrent();
			}
		}

		// re-center the transform containing the bubbles
		// TODO: only do this when the size has actually changed
		test->translate(getWidth(true, true)*0.5f, getHeight(true, true)*0.5f, 0, false);
		
		// interpolate the rotation of the options
		float targetDispayOffset = options.size() > 0 ? ((float)currentOption / options.size() + 0.1f) : 0;
		float delta = targetDispayOffset - displayOffset;
		if(std::abs(delta) > FLT_EPSILON){
			displayOffset += (targetDispayOffset - displayOffset) * 0.2f;
			placeOptions();
		}else if(childrenUpdated){
			placeOptions();
		}
		if(childrenUpdated){
			reorderChildren();
		}
		childrenUpdated = false;
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
		options.back()->eventManager.listeners["selected"].clear();
		unusedOptions.push(options.back());
		options.pop_back();
	}
	currentOption = displayOffset = 0;
	childrenUpdated = true;
}

void PD_UI_Bubble::enable(){
	enabled = true;
	setVisible(true);
}

void PD_UI_Bubble::disable(){
	enabled = false;
	setVisible(false);
}