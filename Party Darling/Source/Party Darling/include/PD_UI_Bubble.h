#pragma once

#include <sweet/UI.h>
#include <shader/ComponentShaderText.h>

#include <NineSlicing.h>

class PD_UI_Bubble : public NodeUI{
public:
	ComponentShaderText * textShader;
	// all of the currently available options
	std::vector<NodeUI *> options;
	// the currently selected option
	// currentOption == options.size() when nothing is selected
	unsigned long int currentOption;

	PD_UI_Bubble(BulletWorld * _world);

	VerticalLinearLayout * vl;
	Transform * test;

	// add an option with the provided _text
	void addOption(std::string _text);

	void select(unsigned long int _option);
	void selectCurrent();

	virtual void update(Step * _step) override;
};