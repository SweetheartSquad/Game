#pragma once

#include <PD_UI_Tasklist.h>
#include <PD_ResourceManager.h>

#include <PD_Scenario.h>
#include <TextLabel.h>
#include <Easing.h>

#define UI_MESSAGE_DURATION 2.f

PD_UI_Tasklist::PD_UI_Tasklist(BulletWorld * _world) :
	NodeUI(_world),
	textShader(new ComponentShaderText(false)),
	font(PD_ResourceManager::scenario->getFont("FONT")->font),
	testID(0)
{
	textShader->setColor(1.f, 1.f, 1.f);

	background->setVisible(false);
	//setVisible(false);

	layout = new VerticalLinearLayout(_world);
	addChild(layout);
	layout->horizontalAlignment = kRIGHT;
	layout->verticalAlignment = kTOP;
	layout->setRationalWidth(1.f, this);
	layout->setRationalHeight(1.f, this);
	layout->setMargin(0.f, 0.6f, 0.05f, 0.05f);
	layout->marginLeft.setRationalSize(1.f, &layout->marginTop);

	layout->setBackgroundColour(0.5f, 0.5f, 0.5f, 0.5f);
	//layout->background->setVisible(true);

	setVisible(false);
}

PD_UI_Tasklist::~PD_UI_Tasklist(){
}

void PD_UI_Tasklist::updateTask(std::string _scenario, int _id, std::string _text, bool _complete){
	auto it = tasks.find(_scenario);
	
	// if the scenario has no tasks, this one must not exist yet, so make it and return early
	if(it == tasks.end()){
		if(!_complete){
			// if the task would have been removed, we don't need to add it
			addTask(_scenario, _id, _text);
		}
		return;
	}

	auto it2 = it->second.find(_id);
	
	// if the scenario has no task with the given id, make it and return early
	if(it2 == it->second.end()){
		if(!_complete){
			// if the task would have been removed, we don't need to add it
			addTask(_scenario, _id, _text);
		}
		return;
	}

	// update the task text
	it2->second->setText(_text);

	// if we're supposed to remove the task, do that last so that the text is up-to-date
	if(_complete){
		removeTask(_scenario, _id);
	}
}

void PD_UI_Tasklist::addTask(std::string _scenario, int _id, std::string _text){
	
	TextLabel * text = new TextLabel(world, font, textShader);
	text->setText(_text);
	layout->addChild(text);
	text->setRationalWidth(1.f, layout);
	text->setHeight(font->getLineHeight() * 1.2f);

	if(tasks.find(_scenario) == tasks.end()){
		std::map<int, TextLabel *> m;
		tasks.insert(std::make_pair(_scenario, m));
	}

	if(tasks.at(_scenario).find(_id) == tasks.at(_scenario).end()){
		tasks.at(_scenario).insert(std::make_pair(_id, text));
	}else{
		Log::warn("Task id for this scenario has already been added!");
	}
	

	invalidateLayout();
}

void PD_UI_Tasklist::removeTask(std::string _scenario, int _id){
	if(tasks.find(_scenario) != tasks.end()){
		auto sTasks = tasks.at(_scenario);
		if(sTasks.find(_id) != sTasks.end()){
			TextLabel * text = tasks.at(_scenario).at(_id);
			layout->removeChild(text);

			sTasks.erase(_id);
			if(sTasks.size() == 0){
				tasks.erase(_scenario);
			}
		}
	}
	invalidateLayout();
}

void PD_UI_Tasklist::update(Step * _step){
	NodeUI::update(_step);
}