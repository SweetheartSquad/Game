#pragma once

#include <PD_UI_YellingContest.h>

PD_UI_YellingContest::PD_UI_YellingContest(BulletWorld* _bulletWorld, Scene* _scene, Font * _font, Shader * _textShader) :
	VerticalLinearLayout(_bulletWorld, _scene)
{
	enemyBubble = new TextArea(world, scene, _font, _textShader, 0.9);
	playerBubble = new TextArea(world, scene, _font, _textShader, 0.9);

	addChild(enemyBubble);
	addChild(playerBubble);
}