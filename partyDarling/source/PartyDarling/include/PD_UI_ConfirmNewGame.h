#pragma once

#include <sweet/UI.h>

class PD_UI_Text;

class PD_UI_ConfirmNewGame : public VerticalLinearLayout{
public:

	PD_UI_Text * btnConfirm;
	PD_UI_Text * btnCancel;


	
	PD_UI_ConfirmNewGame(BulletWorld * _world, ComponentShaderText * _textShader);

	void enable();
	void disable();
};