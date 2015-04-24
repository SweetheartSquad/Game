#pragma once

#include <Box2D\Box2D.h>

class PD_TestScene;

//main collision call back function
class PD_ContactListener : public b2ContactListener{
public:
	PD_ContactListener(PD_TestScene * _scene);

	float damageScoreMult;

	//ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
	virtual void BeginContact(b2Contact* _contact) override;

	virtual void playerBoundaryContact(b2Contact * _contact, b2Fixture * _playerFixture, b2Fixture * _boundaryFixture);

	virtual void EndContact(b2Contact* _contact) override;

	PD_TestScene * scene;
};
