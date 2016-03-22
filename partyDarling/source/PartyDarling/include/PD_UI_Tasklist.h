#pragma once

#include <sweet/UI.h>

class PD_Scenario;

class PD_UI_Tasklist : public NodeUI{
private:
	Font * font;
	ComponentShaderText * textShader;
	std::map<std::string, std::map<int, TextLabel *>> tasks;

	VerticalLinearLayout * layout;

public:
	int testID;

	PD_UI_Tasklist(BulletWorld * _world);
	~PD_UI_Tasklist();

	void addTask(std::string _scenario, int _id, std::string _text);
	void removeTask(std::string _scenario, int _id);

	void animate(float _p);

	virtual void update(Step * _step) override;
};