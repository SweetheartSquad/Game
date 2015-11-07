#pragma once

#include <PD_UI_YellingContest.h>
#include <VerticalLinearLayout.h>
#include <TextArea.h>

class Shader;

class PD_UI_YellingContest : public VerticalLinearLayout{
public:
	TextArea * enemyBubble;
	TextArea * playerBubble;


	PD_UI_YellingContest(BulletWorld * _bulletWorld, Scene * _scene, Font * _font, Shader * _textShader);
};