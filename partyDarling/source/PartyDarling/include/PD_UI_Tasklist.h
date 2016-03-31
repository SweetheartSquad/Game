#pragma once

#include <sweet/UI.h>

class Timeout;
class PD_Scenario;

class PD_UI_Task : public NodeUI{
private:
public:
	VerticalLinearLayout * checkBox;
	NodeUI * checkMark;
	TextArea * text;

	ComponentShaderText * textShader;

	Timeout * addTimeout;
	Timeout * checkTimeout;

	PD_UI_Task(BulletWorld * _world, Font * _font, ComponentShaderText * _textShader);
	~PD_UI_Task();

	void setTextShader(ComponentShaderText * _textShader);
};

class PD_UI_Tasklist : public NodeUI{
private:
	Font * font;
	Font * crossedFont;
	ComponentShaderText * textShader;
	ComponentShaderText * crossedTextShader;

	int numTasks;
	std::map<std::string, std::map<int, PD_UI_Task *>> tasks;

	Texture * texOpen;
	Texture * texClosed;
	Texture * texNew;

	
	NodeUI * icon;
	TextLabel * count;

	bool unseenTask;
public:
	NodeUI_NineSliced * journalLayout;
	VerticalLinearLayout * taskLayout;
	int testID;

	PD_UI_Tasklist(BulletWorld * _world);
	~PD_UI_Tasklist();

	void updateTask(std::string _scenario, int _id, std::string _text, bool _complete);
	void addTask(std::string _scenario, int _id, std::string _text);
	void removeTask(std::string _scenario, int _id);

	void expand();
	void collapse();
	bool isExpanded();

	void incrementCount(int _increment);

	void animate(float _p);

	virtual void update(Step * _step) override;
};