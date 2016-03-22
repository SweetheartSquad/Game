#pragma once

#include <sweet/UI.h>

class PD_Scenario;

class PD_UI_Tasklist : public NodeUI{
private:
	Font * font;
	ComponentShaderText * textShader;

	int numTasks;
	std::map<std::string, std::map<int, TextLabel *>> tasks;

	Texture * texOpen;
	Texture * texClosed;
	Texture * texNew;

	VerticalLinearLayout * journalLayout;
	NodeUI * icon;
	TextLabel * count;

	bool unseenTask;
public:
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