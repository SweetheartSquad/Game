#pragma once

#include <DialogueText.h>
#include <PD_ResourceManager.h>
#include <CharacterUtils.h>

DialogueTextLabel::DialogueTextLabel(BulletWorld * _world, Font * _font, Shader * _textShader) :
	TextLabel(_world, _font, _textShader, 1.f)
{

}

DialogueTextLabel::~DialogueTextLabel(){
	while(timers.size() > 0){
		delete timers.back();
		timers.pop_back();
	}
}

void DialogueTextLabel::tickerIn(float _delay){
	while(timers.size() > 0){
		delete timers.back();
		timers.pop_back();
	}

	for(unsigned long int i = 0; i < usedGlyphs.size(); ++i){
		UIGlyph * g = usedGlyphs.at(i);
		g->setVisible(false);
		Timeout * t = new Timeout(_delay * i);
		t->onCompleteFunction = [g, i](Timeout * _this){
			/*OpenAL_Sound * voice = PD_ResourceManager::scenario->getAudio(PD_ResourceManager::speaker)->sound;
			g->setVisible(true);
			//std::cout << g->character/128.f+0.5f << std::endl;
			if(i % 2 == 0 && iswalpha(g->character)){
				voice->setPitch(g->character/178.f+0.75f);
				voice->setGain(PD_ResourceManager::getSfxVolume());
				voice->play();
			}*/
		};
		t->start();
		timers.push_back(t);
	}
}

void DialogueTextLabel::finishTicking(){
	while(timers.size() > 0){
		timers.back()->trigger();
		delete timers.back();
		timers.pop_back();
	}
}

void DialogueTextLabel::update(Step * _step){
	TextLabel::update(_step);
	for(signed long int i = timers.size()-1; i >= 0; --i){
		timers.at(i)->update(_step);
		if(timers.at(i)->complete){
			delete timers.at(i);
			timers.erase(timers.begin() + i);
		}
	}
}

DialogueTextArea::DialogueTextArea(BulletWorld * _world, Font * _font, Shader * _textShader) :
	TextArea(_world, _font, _textShader, 1.f),
	ticking(false)
{

}

DialogueTextArea::~DialogueTextArea(){
}

void DialogueTextArea::tickerIn(float _delay){
	ticking = true;
	while(timers.size() > 0){
		delete timers.back();
		timers.pop_back();
	}

	float time = 0;
	for(unsigned long int i = 1; i < usedLines.size(); ++i){
		DialogueTextLabel * tl = dynamic_cast<DialogueTextLabel *>(usedLines.at(i));
		tl->setVisible(false);
		time += usedLines.at(i-1)->usedGlyphs.size() * _delay;
		Timeout * t = new Timeout(time);
		t->onCompleteFunction = [tl, _delay](Timeout * _this){
			tl->setVisible(true);
			tl->tickerIn(_delay);
		};
		t->start();
		timers.push_back(t);
	}
	for(unsigned long int i = 0; i < usedLines.size(); ++i){
		dynamic_cast<DialogueTextLabel *>(usedLines.at(i))->tickerIn(_delay);
		time += usedLines.at(i)->usedGlyphs.size() * _delay;
	}
	Timeout * t = new Timeout(time);
	t->onCompleteFunction = [this](Timeout * _this){
		this->ticking = false;
	};
	t->start();
	timers.push_back(t);
}

void DialogueTextArea::finishTicking(){
	while(timers.size() > 0){
		timers.back()->trigger();
		delete timers.back();
		timers.pop_back();
	}
	for(unsigned long int i = 0; i < usedLines.size(); ++i){
		dynamic_cast<DialogueTextLabel *>(usedLines.at(i))->finishTicking();
	}
	ticking = false;
}

TextLabel * DialogueTextArea::getNewLine(){
	TextLabel * line;
	if(unusedLines.size() > 0) {
		line = unusedLines.back();
		unusedLines.pop_back();
	}else{
		line = new DialogueTextLabel(world, font, textShader);
		line->setRationalWidth(1.f, this);
		line->horizontalAlignment = horizontalAlignment;
	}
	usedLines.push_back(line);
	addChild(line);
	return line;
}


void DialogueTextArea::update(Step * _step){
	TextArea::update(_step);
	for(signed long int i = timers.size()-1; i >= 0; --i){
		timers.at(i)->update(_step);
		if(timers.at(i)->complete){
			delete timers.at(i);
			timers.erase(timers.begin() + i);
		}
	}
}