#pragma once

#include <PD_UI_Message.h>
#include <PD_ResourceManager.h>

#include <PD_Item.h>
#include <Timeout.h>
#include <Easing.h>

#define UI_MESSAGE_DURATION 2.f

PD_UI_Message::PD_UI_Message(BulletWorld * _world) :
	VerticalLinearLayout(_world),
	textShader(new ComponentShaderText(false))
{
	setVisible(false);
	// Pickup/Remove Item Animaiton
	horizontalAlignment = kCENTER;
	verticalAlignment = kMIDDLE;

	HorizontalLinearLayout * imageContainer = new HorizontalLinearLayout(_world);
	addChild(imageContainer);
	imageContainer->boxSizing = kCONTENT_BOX;
	imageContainer->horizontalAlignment = kCENTER;
	imageContainer->verticalAlignment = kMIDDLE;
	imageContainer->setRationalHeight(0.2f, this);
	imageContainer->setSquareWidth(1.f);
	imageContainer->setMarginBottom(0.05f);

	image = new NodeUI(_world);
	imageContainer->addChild(image);
	image->setRationalHeight(1.f, imageContainer);
	image->setSquareWidth(1.f);
	image->background->mesh->setScaleMode(GL_NEAREST);

	textBubble = new NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("MESSAGE-BUBBLE"));
	textBubble->setScaleMode(GL_NEAREST);
	addChild(textBubble);
	textBubble->setBorder(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight());
	textBubble->setPixelHeight(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight() * 2.5f);
	textBubble->setRationalWidth(0.5f, this);

	text = new TextLabel(_world, PD_ResourceManager::scenario->getFont("FONT")->font, textShader);
	textBubble->addChild(text);
	text->setRationalWidth(1.f, textBubble);
	text->setRationalHeight(1.f, textBubble);
	text->horizontalAlignment = kCENTER;
	text->verticalAlignment = kMIDDLE;

	// Initial setup
	image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
	textBubble->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
	textShader->setColor(1.f, 1.f, 1.f, 0.f);
}

PD_UI_Message::~PD_UI_Message(){
	for(auto t : animationTimeouts){
		t->stop();
		delete t;
	}
	animationTimeouts.clear();
	delete textShader;
}

void PD_UI_Message::gainItem(PD_Item * _item){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
		image->setSquareWidth(1.f);
		invalidateLayout();
	});
	t->eventManager->addEventListener("start", [this, _item](sweet::Event * _event){
		setVisible(true);

		setItemTexture(_item);
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

void PD_UI_Message::loseItem(PD_Item * _item){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
		image->setSquareWidth(1.f);
		invalidateLayout();
	});
	t->eventManager->addEventListener("start", [this, _item](sweet::Event * _event){
		setVisible(true);

		setItemTexture(_item);
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

void PD_UI_Message::gainLifeToken(std::string _name, Texture * _tex){
	Timeout * t = new Timeout(UI_MESSAGE_DURATION, [this](sweet::Event * _event){
		setVisible(false);
	});
	t->eventManager->addEventListener("start", [this, _name, _tex](sweet::Event * _event){
		setVisible(true);

		image->background->mesh->replaceTextures(_tex);

		image->setSquareWidth(1.f);
		invalidateLayout();

		++_tex->referenceCount;

		text->setText("Acquired " + _name + "'s Friendship");

		image->setBackgroundColour(1.f, 1.f, 1.f, 0.f);
		textShader->setColor(1.f, 1.f, 1.f, 0.f);
	});
	t->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		animate(p);
	});

	animationTimeouts.push_back(t);
}

void PD_UI_Message::displayMessage(std::string _message){
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

void PD_UI_Message::setItemTexture(PD_Item * _item){
	// make sure the item is displayed at the correct size
	AssetTexture * assetTex = PD_ResourceManager::itemTextures->getTexture(_item->definition->texture);
	assetTex->load();
	Texture * tex = assetTex->texture;

	image->background->mesh->replaceTextures(tex);

	image->setSquareWidth((float)tex->width/tex->height);

	invalidateLayout();
}

void PD_UI_Message::animate(float _p){
	float a;
	if(_p  <= 0.5f){
		a = Easing::easeOutCubic(_p, 0.f, 1.f, 0.5f);
	}else if(_p < 0.8f){
		a = 1.f;
	}else{
		a = Easing::easeInCubic(_p - 0.8f, 1.f, -1.f, 0.2f);
	}

	image->setBackgroundColour(1.f, 1.f, 1.f, a);
	textBubble->setBackgroundColour(1.f, 1.f, 1.f, a);
	textShader->setColor(1.f, 1.f, 1.f, a);
}

void PD_UI_Message::update(Step * _step){
	if(animationTimeouts.size() > 0){
		Timeout * t = animationTimeouts.front();
		if(!t->active){
			t->start();
			t = nullptr;
		}else if(t->complete){
			// Update the event manager with the complete event (in this case, it does the same thing as what should be done at progress = 1, which is hiding everything)
			Step s;
			s.setDeltaTime(0.f);
			t->update(&s);
			delete t;
			animationTimeouts.erase(animationTimeouts.begin());
			t = nullptr;
		}

		// only update if not starting or completing?
		if(t != nullptr){
			t->update(_step);
		}
	}
	VerticalLinearLayout::update(_step);
}