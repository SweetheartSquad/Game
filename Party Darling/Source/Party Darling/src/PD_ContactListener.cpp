#include <PD_ContactListener.h>
#include <PD_TestScene.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>
#include <PD_Player.h>

#include "Scene.h"
#include "Box2dWorld.h"
#include "Box2DSprite.h"
#include "Box2DSuperSprite.h"
#include <Box2D/Box2D.h>
#include <SoundManager.h>

#include <Box2D\Dynamics\Joints\b2RevoluteJoint.h>

PD_ContactListener::PD_ContactListener(PD_TestScene * _scene) :
scene(_scene),
damageScoreMult(0.f)
{
	_scene->box2dWorld->b2world->SetContactListener(this);
}

void PD_ContactListener::BeginContact(b2Contact * _contact){
	b2Filter fA = _contact->GetFixtureA()->GetFilterData();
	b2Filter fB = _contact->GetFixtureB()->GetFilterData();
	b2Fixture * playerFixture = nullptr;
	b2Fixture * otherFixture = nullptr;
	if ((fA.categoryBits & PD_Game::kPLAYER) != 0){
		playerFixture = _contact->GetFixtureA();
		otherFixture = _contact->GetFixtureB();
	}
	else if ((fB.categoryBits & PD_Game::kPLAYER) != 0){
		playerFixture = _contact->GetFixtureB();
		otherFixture = _contact->GetFixtureA();
	}

	if (playerFixture != nullptr){
		if ((fA.categoryBits & PD_Game::kBOUNDARY) != 0 || (fB.categoryBits & PD_Game::kBOUNDARY) != 0){
			playerBoundaryContact(_contact, playerFixture, otherFixture);
		}
	}else{
		
	}
}


void PD_ContactListener::EndContact(b2Contact* _contact){
	b2Filter fA = _contact->GetFixtureA()->GetFilterData();
	b2Filter fB = _contact->GetFixtureB()->GetFilterData();

	b2Fixture * playerFixture = nullptr;
	PD_Player * player = nullptr;
	if ((fA.categoryBits & PD_Game::kPLAYER) != 0){
		playerFixture = _contact->GetFixtureA();
		//We pretty much know its a PD_ character because of the category bits
		player = static_cast<PD_Player *>(playerFixture->GetUserData());
		if (player != nullptr){
			//	NodeTransformable * s = reinterpret_cast<NodeTransformable *>(player);
			/*if ((fB.categoryBits & PD_Game::kPLAYER) != 0) {
				player->removeCollision(PD_Game::kPLAYER);
			}
			if ((fB.categoryBits & PD_Game::kBOUNDARY) != 0) {
				player->removeCollision(PD_Game::kBOUNDARY);
			}
			if ((fB.categoryBits & PD_Game::kENEMY) != 0) {
				player->removeCollision(PD_Game::kENEMY);
			}
			if ((fB.categoryBits & PD_Game::kBUMPER) != 0) {
				player->removeCollision(PD_Game::kBUMPER);
			}*/
		}
	}
	if ((fB.categoryBits & PD_Game::kPLAYER) != 0){
		playerFixture = _contact->GetFixtureB();
		player = static_cast<PD_Player *>(playerFixture->GetUserData());
		/*if (player != nullptr) {
			if ((fA.categoryBits & PD_Game::kPLAYER) != 0) {
				player->removeCollision(PD_Game::kPLAYER);
			}
			if ((fA.categoryBits & PD_Game::kBOUNDARY) != 0) {
				player->removeCollision(PD_Game::kBOUNDARY);
			}
			if ((fA.categoryBits & PD_Game::kENEMY) != 0) {
				player->removeCollision(PD_Game::kENEMY);
			}
			if ((fA.categoryBits & PD_Game::kBUMPER) != 0) {
				player->removeCollision(PD_Game::kBUMPER);
			}
		}*/
	}

	if (playerFixture != nullptr){
		PD_Player * player = static_cast<PD_Player *>(playerFixture->GetUserData());
	}

}

void PD_ContactListener::playerBoundaryContact(b2Contact * _contact, b2Fixture * _playerFixture, b2Fixture * _boundaryFixture){
	/*PD_Player * player = static_cast<PD_Player*>(player_fixture->GetUserData());
	if (player != nullptr) {
		PD_ResourceManager::miscSounds->play("bumper");
	}*/
}
