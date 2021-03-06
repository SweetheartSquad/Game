#pragma once

#include <PD_UI_Tasklist.h>
#include <PD_ResourceManager.h>

#include <PD_Scenario.h>
#include <TextLabel.h>
#include <Timeout.h>
#include <Easing.h>

#define TASKLIST_OPACITY 0.7f

PD_UI_Task::PD_UI_Task(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader):
	NodeUI(_world)
{
	setRenderMode(kTEXTURE);
	background->setVisible(false);
	setAlpha(0.f);

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
	checkContainer->background->setVisible(false);

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
	text->background->setVisible(false);

	addTimeout = new Timeout(1.f, [this](sweet::Event * _event){
		setAlpha(1.f);
	});
	addTimeout->eventManager->addEventListener("start", [this](sweet::Event * _event){
		setAlpha(0.f);
	});

	addTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		setAlpha(p);
	});
	childTransform->addChild(addTimeout, false);

	checkTimeout = new Timeout(0.5f, [this](sweet::Event * _event){
		checkMark->setRationalWidth(1.f, checkMark->nodeUIParent);
		invalidateLayout();
		invalidateRenderFrame();
	});
	checkTimeout->eventManager->addEventListener("start", [this](sweet::Event * _event){
		checkMark->setVisible(true);
		invalidateLayout();
		invalidateRenderFrame();
	});
	checkTimeout->eventManager->addEventListener("progress", [this](sweet::Event * _event){
		float p = _event->getFloatData("progress");
		p = Easing::easeOutElastic(p, 0.f, 1.f, checkTimeout->targetSeconds, -1, 4.f);
		checkMark->setRationalWidth(p, checkMark->nodeUIParent);
		invalidateLayout();
		invalidateRenderFrame();
	});
	childTransform->addChild(checkTimeout, false);
}

PD_UI_Task::~PD_UI_Task(){
}

void PD_UI_Task::setTextShader(ComponentShaderText * _textShader){
	text->setShader(_textShader);
}

PD_UI_Tasklist::PD_UI_Tasklist(BulletWorld * _world) :
	NodeUI(_world),
	textShader(new ComponentShaderText(true)),
	crossedTextShader(new ComponentShaderText(true)),
	font(PD_ResourceManager::scenario->getFont("TASKLIST-FONT")->font),
	crossedFont(PD_ResourceManager::scenario->getFont("TASKLIST-FONT-CROSSED")->font),
	unseenTask(false),
	numTasks(0),
	testID(0)
{
	textShader->setColor(1.f, 1.f, 1.f);
	textShader->incrementReferenceCount();
	textShader->name = "PD_UI_Tasklist text shader (normal)";

	crossedTextShader->setColor(0.f, 0.f, 0.f);
	crossedTextShader->incrementReferenceCount();
	crossedTextShader->name = "PD_UI_Tasklist text shader (crossed)";

	background->setVisible(false);

	NodeUI * container = new NodeUI(_world);
	addChild(container);
	container->setRationalWidth(1.f, this);
	container->setRationalHeight(1.f, this);
	container->setMargin(0.f, 0.66f, 0.05f, 0.05f);
	container->marginLeft.setRationalSize(1.f, &container->marginTop);
	container->background->setVisible(false);

	icon = new NodeUI(_world);
	icon->boxSizing = kCONTENT_BOX;
	container->addChild(icon);
	icon->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("JOURNAL-OPEN")->texture);
	icon->background->mesh->setScaleMode(GL_NEAREST);
	icon->setHeight(0.05f);
	icon->setSquareWidth(190.f/74.f);
	icon->setMarginBottom(0.95f);

	VerticalLinearLayout * layout = new VerticalLinearLayout(_world);
	container->addChild(layout);
	layout->horizontalAlignment = kLEFT;
	layout->verticalAlignment = kTOP;
	layout->setRationalWidth(1.f, container);
	layout->setRationalHeight(0.95f, container);

	journalLayout = new NodeUI_NineSliced(_world, PD_ResourceManager::scenario->getNineSlicedTexture("MESSAGE-BUBBLE"));
	layout->addChild(journalLayout);
	journalLayout->setScaleMode(GL_NEAREST);
	journalLayout->setBorder(PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight() * 0.5f);
	journalLayout->setRationalWidth(1.f, layout);
	journalLayout->setAutoresizeHeight();
	journalLayout->setBackgroundColour(1.f, 1.f, 1.f, TASKLIST_OPACITY);

	float lineHeight = PD_ResourceManager::scenario->getFont("FONT")->font->getLineHeight();

	taskLayout = new VerticalLinearLayout(_world);
	taskLayout->boxSizing = kCONTENT_BOX;
	journalLayout->addChild(taskLayout);
	taskLayout->horizontalAlignment = kLEFT;
	taskLayout->verticalAlignment = kTOP;
	taskLayout->setRationalWidth(1.f, journalLayout);
	taskLayout->setAutoresizeHeight();
	taskLayout->setPadding(0.f, lineHeight * 0.5f);

	journalLayout->setVisible(false);
}

PD_UI_Tasklist::~PD_UI_Tasklist(){
	textShader->decrementAndDelete();
	crossedTextShader->decrementAndDelete();
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
	if(numTasks == 0 && icon->isVisible()){
		journalLayout->setVisible(true);
	}

	if(tasks.find(_scenario) == tasks.end()){
		std::map<int, PD_UI_Task *> m;
		tasks.insert(std::make_pair(_scenario, m));
	}

	if(tasks.at(_scenario).find(_id) == tasks.at(_scenario).end()){
		PD_UI_Task * task = new PD_UI_Task(world, font, textShader);
		task->setRationalWidth(1.f, taskLayout);
		task->setHeight(font->getLineHeight() * 3.f);
		taskLayout->addChild(task);
		task->text->setText(_text);

		task->addTimeout->eventManager->addEventListener("start", [this](sweet::Event * _event){
			std::stringstream s;
			s << "journalLayout: " << journalLayout << " taskLayout: " << taskLayout->height.getSize();
			Log::info(s.str());
		});

		tasks.at(_scenario).insert(std::make_pair(_id, task));

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

			task->setTextShader(crossedTextShader);
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
}

void PD_UI_Tasklist::expand(){
	icon->setVisible(true);
	journalLayout->setVisible(numTasks > 0);
	//count->setVisible(false);
}

void PD_UI_Tasklist::collapse(){
	icon->setVisible(false);
	journalLayout->setVisible(false);
	//count->setVisible(numTasks > 0);
}

bool PD_UI_Tasklist::isExpanded(){
	return icon->isVisible();
}