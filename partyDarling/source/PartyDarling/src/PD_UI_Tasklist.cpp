#pragma once

#include <PD_UI_Tasklist.h>
#include <PD_ResourceManager.h>

#include <PD_Scenario.h>
#include <TextLabel.h>
#include <Easing.h>

PD_UI_Tasklist::PD_UI_Tasklist(BulletWorld * _world) :
	NodeUI(_world),
	textShader(new ComponentShaderText(false)),
	font(PD_ResourceManager::scenario->getFont("TASKLIST-FONT")->font),
	unseenTask(false),
	numTasks(0),
	testID(0)
{
	textShader->setColor(1.f, 1.f, 1.f);

	background->setVisible(false);

	texOpen = PD_ResourceManager::scenario->getTexture("JOURNAL-OPEN")->texture;
	texClosed = PD_ResourceManager::scenario->getTexture("JOURNAL-CLOSED")->texture;
	texNew = PD_ResourceManager::scenario->getTexture("JOURNAL-NEW")->texture;

	VerticalLinearLayout * layout = new VerticalLinearLayout(_world);
	addChild(layout);
	layout->horizontalAlignment = kLEFT;
	layout->verticalAlignment = kTOP;
	layout->setRationalWidth(1.f, this);
	layout->setRationalHeight(1.f, this);
	layout->setMargin(0.f, 0.6f, 0.05f, 0.05f);
	layout->marginLeft.setRationalSize(1.f, &layout->marginTop);

	icon = new NodeUI(_world);
	layout->addChild(icon);
	icon->background->mesh->pushTexture2D(texOpen);
	icon->background->mesh->setScaleMode(GL_NEAREST);
	icon->setHeight(0.05f);
	icon->setSquareWidth(1.f);

	count = new TextLabel(_world, PD_ResourceManager::scenario->getFont("TASKCOUNT-FONT")->font, textShader);
	count->boxSizing = kCONTENT_BOX;
	icon->addChild(count);
	count->horizontalAlignment = kCENTER;
	count->verticalAlignment = kMIDDLE;
	count->setHeight(font->getLineHeight() * 1.2);
	count->setSquareWidth(1.f);
	count->setMarginBottom(0.75f);
	count->setMarginLeft(0.75f);
	count->setBackgroundColour(1.f, 0.f, 0.f);
	count->background->setVisible(true);
	count->setVisible(false);

	journalLayout = new VerticalLinearLayout(_world);
	layout->addChild(journalLayout);
	journalLayout->horizontalAlignment = kLEFT;
	journalLayout->verticalAlignment = kTOP;
	journalLayout->setRationalWidth(1.f, layout);
	journalLayout->setRationalHeight(0.95f, layout);
	journalLayout->setBackgroundColour(0.5f, 0.5f, 0.5f, 0.5f);
	journalLayout->background->setVisible(false);
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

	if(tasks.find(_scenario) == tasks.end()){
		std::map<int, TextLabel *> m;
		tasks.insert(std::make_pair(_scenario, m));
	}

	if(tasks.at(_scenario).find(_id) == tasks.at(_scenario).end()){
		TextLabel * text = new TextLabel(world, font, textShader);
		text->setText(_text);
		journalLayout->addChild(text);
		text->setRationalWidth(1.f, journalLayout);
		text->setHeight(font->getLineHeight() * 1.2f);

		tasks.at(_scenario).insert(std::make_pair(_id, text));

		// add indicator if task list is collapsed
		if(!journalLayout->isVisible()){
			icon->background->mesh->replaceTextures(texNew);
		}

		incrementCount(1);
		invalidateLayout();
	}else{
		Log::warn("Task id for this scenario has already been added!");
	}
}

void PD_UI_Tasklist::removeTask(std::string _scenario, int _id){
	if(tasks.find(_scenario) != tasks.end()){
		auto sTasks = tasks.at(_scenario);
		if(sTasks.find(_id) != sTasks.end()){
			TextLabel * text = tasks.at(_scenario).at(_id);
			journalLayout->removeChild(text);

			sTasks.erase(_id);
			if(sTasks.size() == 0){
				tasks.erase(_scenario);
			}
		}
	}
	incrementCount(-1);
	invalidateLayout();
}

void PD_UI_Tasklist::update(Step * _step){
	NodeUI::update(_step);
}

void PD_UI_Tasklist::incrementCount(int _increment){
	numTasks += _increment;

	if(numTasks > 0){
		count->setVisible(!journalLayout->isVisible());
		std::stringstream s;
		s << numTasks;
		count->setText(s.str());
	}else{
		count->setVisible(false);
	}
}

void PD_UI_Tasklist::expand(){
	journalLayout->setVisible(true);
	icon->background->mesh->replaceTextures(texOpen);
	count->setVisible(false);
}

void PD_UI_Tasklist::collapse(){
	journalLayout->setVisible(false);
	icon->background->mesh->replaceTextures(texClosed);
	count->setVisible(numTasks > 0);
}

bool PD_UI_Tasklist::isExpanded(){
	return journalLayout->isVisible();
}