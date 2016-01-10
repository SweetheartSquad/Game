#pragma once

#include <sweet/UI.h>
#include <shader/ComponentShaderText.h>

#include <NineSlicing.h>

class PD_UI_Bubble : public NodeUI{
private:
	float displayOffset;
	Texture_NineSliced * bubbleTex;
public:
	ComponentShaderText * textShader;
	// all of the currently available options
	std::vector<NodeUI_NineSliced *> options;
	// the currently selected option
	// currentOption == options.size() when nothing is selected
	unsigned long int currentOption;
	
	PD_UI_Bubble(BulletWorld * _world);
	~PD_UI_Bubble();

	VerticalLinearLayout * vl;
	Transform * test;

	// add an option with the provided _text
	void addOption(std::string _text);

	void select(unsigned long int _option);
	void selectCurrent();

	// select the next option
	void next();
	// select the previous option
	void prev();
	
	// re-order children so that the currently selected is the last child
	// (this makes it so that it draws on top of the others when depth-testing is turned off)
	void reorderChildren();

	virtual void update(Step * _step) override;
};