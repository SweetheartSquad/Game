#pragma once

#include <PD_UI_Tasklist.h>
#include <PD_ResourceManager.h>

#include <PD_Scenario.h>
#include <TextLabel.h>
#include <Timeout.h>
#include <Easing.h>

#define TASKLIST_OPACITY 0.7f

PD_UI_Task::PD_UI_Task(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader):
	NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("TASK-BG")),
	textShader(_textShader),
	isAnimating(false)
{
	setBackgroundColour(1.f, 1.f, 1.f, TASKLIST_OPACITY);
	setScaleMode(GL_NEAREST);
	setBorder(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight() * 0.5f);

	NodeUI * container = new NodeUI(_world);
	addChild(container);
	container->setRationalWidth(1.f, this);
	container->setRationalHeight(1.f, this);
	container->background->setVisible(false);

	VerticalLinearLayout * checkContainer = checkBox = new VerticalLinearLayout(world);
	container->addChild(checkContainer);
	checkContainer->horizontalAlignment = kCENTER;
	checkContainer->verticalAlignment = kMIDDLE;
	checkContainer->setWidth(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight());
	checkContainer->setRationalHeight(1.f, container);

	checkBox = new VerticalLinearLayout(world);
	checkBox->horizontalAlignment = kCENTER;
	checkBox->verticalAlignment = kMIDDLE;
	checkContainer->addChild(checkBox);
	checkBox->setRationalWidth(0.75f, checkContainer);
	checkBox->setSquareHeight(1.f);
	checkBox->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("JOURNAL-CHECK-BOX")->texture);
	checkBox->background->mesh->setScaleMode(GL_NEAREST);
	checkBox->setBackgroundColour(1.f, 1.f, 1.f, 1.f);
	checkBox->background->setVisible(true);

	checkMark = new NodeUI(world);
	checkBox->addChild(checkMark);
	checkMark->setRationalWidth(0.f, checkBox);
	checkMark->setSquareHeight(1.f);
	checkMark->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("JOURNAL-CHECK-MARK")->texture);
	checkMark->background->mesh->setScaleMode(GL_NEAREST);
	checkMark->setVisible(false);
	
	text = new TextArea(world, _font, _textShader);
	text->setWrapMode(kWORD);
	text->verticalAlignment = kMIDDLE;
	container->addChild(text);
	text->setRationalWidth(1.f, container);
	text->setRationalHeight(1.f, container);
	text->marginLeft.setRationalSize(1.f, &checkContainer->width);

	addTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		textShader->setColor(textShader->getColor().r, textShader->getColor().g, textShader->getColor().b, 1.f);
		eventManager->triggerEvent("taskAnimationComplete");
		isAnimating = false;
	});
	addTimeout->eventManager->addEventListener("start", [this](sweet::Event * _event){
		if(!isAnimating){ // In case restart happens before complete event
			isAnimating = true;
			eventManager->triggerEvent("taskAnimationStart");
		}
	});

	addTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		textShader->setColor(textShader->getColor().r, textShader->getColor().g, textShader->getColor().b, p);
		invalidateLayout();
	});
	childTransform->addChild(addTimeout, false);

	checkTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		checkMark->setRationalWidth(1.f, checkMark->nodeUIParent);
	});
	checkTimeout->eventManager->addEventListener("start", [this](sweet::Event * _event){
		checkMark->setVisible(true);
	});
	checkTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		p = Easing::easeOutElastic(p, 0.f, 1.f, checkTimeout->targetSeconds, -1, 4.f);
		checkMark->setRationalWidth(p, checkMark->nodeUIParent);
		invalidateLayout();
	});
	childTransform->addChild(checkTimeout, false);
}

PD_UI_Task::~PD_UI_Task(){
}

void PD_UI_Task::setTextShader(ComponentShaderText * _textShader, bool _deleteOldShader){
	text->setShader(_textShader, true);
	
	if(_deleteOldShader){
		delete textShader;
	}
	textShader = _textShader;
}

PD_UI_Tasklist::PD_UI_Tasklist(BulletWorld * _world) :
	NodeUI(_world),
	textShader(new ComponentShaderText(false)),
	crossedTextShader(new ComponentShaderText(false)),
	font(PD_ResourceManager::scenario->getFont("TASKLIST-FONT")->font),
	crossedFont(PD_ResourceManager::scenario->getFont("TASKLIST-FONT-CROSSED")->font),
	unseenTask(false),
	numTasks(0),
	testID(0),
	playingAnimations(0)
{
	textShader->setColor(1.f, 1.f, 1.f);
	crossedTextShader->setColor(0.f, 0.f, 0.f);

	setRenderMode(kTEXTURE);
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
	layout->setMargin(0.f, 0.66f, 0.05f, 0.05f);
	layout->marginLeft.setRationalSize(1.f, &layout->marginTop);

	icon = new NodeUI(_world);
	layout->addChild(icon);
	icon->boxSizing = kCONTENT_BOX;
	icon->background->mesh->pushTexture2D(texOpen);
	icon->background->mesh->setScaleMode(GL_NEAREST);
	icon->setHeight(0.05f);
	icon->setSquareWidth(190.f/74.f);
	icon->setMarginBottom(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight() * 0.5f);

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

	float lineHeight = PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight();
	
	top = new NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("MESSAGE-BUBBLE"));
	journalLayout->addChild(top);
	top->setRationalWidth(1.f, journalLayout);
	top->setHeight(lineHeight * 0.5f);
	top->setBorder(lineHeight * 0.5f, lineHeight * 0.5f, 0.f, lineHeight * 0.5f);
	top->setBackgroundColour(1.f, 1.f, 1.f, TASKLIST_OPACITY);
	top->setVisible(false);

	taskLayout = new VerticalLinearLayout(_world);
	journalLayout->addChild(taskLayout);
	taskLayout->horizontalAlignment = kLEFT;
	taskLayout->verticalAlignment = kTOP;
	taskLayout->setRationalWidth(1.f, journalLayout);
	taskLayout->setAutoresizeHeight();
	taskLayout->setBackgroundColour(0.5f, 0.5f, 0.5f, 0.5f);
	taskLayout->background->setVisible(false);

	bottom = new NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("MESSAGE-BUBBLE"));
	journalLayout->addChild(bottom);
	bottom->setRationalWidth(1.f, journalLayout);
	bottom->setHeight(lineHeight * 0.5);
	bottom->setBorder(lineHeight * 0.5f, lineHeight * 0.5f, lineHeight * 0.5f, 0.f);
	bottom->setBackgroundColour(1.f, 1.f, 1.f, TASKLIST_OPACITY);
	bottom->setVisible(false);
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
	it2->second->text->setText(_text);
	invalidateLayout();
	it2->second->addTimeout->restart();

	// if we're supposed to remove the task, do that last so that the text is up-to-date
	if(_complete){
		removeTask(_scenario, _id);
	}
}

void PD_UI_Tasklist::addTask(std::string _scenario, int _id, std::string _text){
	if(numTasks == 0){
		top->setVisible(true);
		bottom->setVisible(true);
	}

	if(tasks.find(_scenario) == tasks.end()){
		std::map<int, PD_UI_Task *> m;
		tasks.insert(std::make_pair(_scenario, m));
	}

	if(tasks.at(_scenario).find(_id) == tasks.at(_scenario).end()){
		ComponentShaderText * shader = new ComponentShaderText(false);
		//shader->setColor(0.98f, 0.74f, 0.42f, 0.f);
		shader->setColor(1.f, 1.f, 1.f, 0.f);

		PD_UI_Task * task = new PD_UI_Task(world, font, shader);
		task->setRationalWidth(1.f, taskLayout);
		task->setHeight(font->getLineHeight() * 3.f);
		taskLayout->addChild(task);
		task->text->setText(_text);

		task->eventManager->addEventListener("taskAnimationStart", [this](sweet::Event * _event){
			if(playingAnimations == 0){
				setRenderMode(kENTITIES);
			}
			++playingAnimations;
		});
		task->eventManager->addEventListener("taskAnimationComplete", [this](sweet::Event * _event){
			--playingAnimations;
			if(playingAnimations == 0){
				setRenderMode(kTEXTURE);
			}
		});

		tasks.at(_scenario).insert(std::make_pair(_id, task));

		// add indicator if task list is collapsed
		if(!journalLayout->isVisible()){
			icon->background->mesh->replaceTextures(texNew);
		}

		task->addTimeout->restart();

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
			PD_UI_Task * task = tasks.at(_scenario).at(_id);

			task->setTextShader(crossedTextShader, true);
			task->text->setFont(crossedFont, true);

			incrementCount(-1);
			invalidateLayout();

			task->checkTimeout->restart();
		}
	}
}

void PD_UI_Tasklist::update(Step * _step){
	if(isVisible()){
		NodeUI::update(_step);
	}
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