#pragma once

#include "TestCharacter.h"
#include "Box2DSprite.h"
#include "Box2DWorld.h"
#include "shader/Shader.h"
#include "Scene.h"
#include "BitmapFont.h"

#include <MeshFactory.h>
#include "Texture.h"
#include <TextureSampler.h>

ComponentTexture::ComponentTexture(Texture * _texture, float _width, float _height) :
	texture(_texture),
	width(_width),
	height(_height)
{
}

TestCharacter::TestCharacter(BulletWorld * _world) :
	BulletMeshEntity(_world, MeshFactory::getCubeMesh()),
	head(nullptr),
	leftUpperArm(nullptr),
	leftLowerArm(nullptr),
	leftHand(nullptr),
	rightUpperArm(nullptr),
	rightLowerArm(nullptr),
	rightHand(nullptr),
	leftUpperLeg(nullptr),
	leftLowerLeg(nullptr),
	rightUpperLeg(nullptr),
	rightLowerLeg(nullptr)
{
	box2dWorld = new Box2DWorld();
	torso = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelTorso.png", true, true), 150, 360));
	head = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelHead.png", true, true), 160, 270));
	leftUpperArm = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelUpperArm.png", true, true), 55, 205));
	rightUpperArm = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelUpperArm.png", true, true), 55, 205));
	leftLowerArm = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelLowerArm.png", true, true), 40, 145));
	rightLowerArm = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelLowerArm.png", true, true), 40, 145));
	leftHand = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelHand.png", true, true), 50, 82));
	rightHand = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelHand.png", true, true), 50, 82));
	leftUpperLeg = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelUpperLeg.png", true, true), 70, 215));
	rightUpperLeg = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelUpperLeg.png", true, true), 70, 215));
	leftLowerLeg = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelLowerLeg.png", true, true), 135, 285));
	rightLowerLeg = new Box2DSprite(box2dWorld, new TextureSampler(new Texture("assets/textures/character components/MichaelLowerLeg.png", true, true), 135, 285));
	
	ratioX_neck_to_torso = 0.0f;
	ratioY_neck_to_torso = 0.8f;
	ratioX_torso_to_neck = 0.0f;
	ratioY_torso_to_neck = 0.8f;
	
	ratioX_shoulder_to_torso = 0.f;
	ratioY_shoulder_to_torso = 1.f;
	ratioX_torso_to_shoulder = 0.8f;
	ratioY_torso_to_shoulder = 0.5f;
	
	ratioX_elbow_to_shoulder = 0.f;
	ratioY_elbow_to_shoulder = 0.7f;
	ratioX_shoulder_to_elbow = 0.f;
	ratioY_shoulder_to_elbow = 0.7f;
	
	ratioX_wrist_to_elbow = 0.f;
	ratioY_wrist_to_elbow = 0.8f;
	ratioX_elbow_to_wrist = 0.f;
	ratioY_elbow_to_wrist = 0.8f;
	
	ratioX_hip_to_torso = 0.f;
	ratioY_hip_to_torso = 1.f;
	ratioX_torso_to_hip = 0.55f;
	ratioY_torso_to_hip = 0.7f;
	
	ratioX_knee_to_hip = 1.0f;
	ratioY_knee_to_hip = 0.8f;
	ratioX_hip_to_knee = 1.0f;
	ratioY_hip_to_knee = 0.8f;

	components.push_back(&head);
	components.push_back(&leftUpperArm);
	components.push_back(&leftLowerArm);
	components.push_back(&leftHand);
	components.push_back(&rightUpperArm);
	components.push_back(&rightLowerArm);
	components.push_back(&rightHand);
	components.push_back(&leftUpperLeg);
	components.push_back(&leftLowerLeg);
	components.push_back(&rightUpperLeg);
	components.push_back(&rightLowerLeg);
	components.push_back(&torso);

	setColliderAsCapsule(0.5f, 5.f);
	createRigidBody(1);
}

TestCharacter::~TestCharacter(){
}

void TestCharacter::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderStack){
	MeshEntity::render(_matrixStack, _renderStack);
}

void TestCharacter::update(Step * _step){
	box2dWorld->update(_step);
	BulletMeshEntity::update(_step);
	//if(reactiveBody){
		b2RevoluteJoint * neck = ((b2RevoluteJoint *)head->body->GetJointList()->joint);
		float angle = neck->GetJointAngle();

		neck->SetMotorSpeed(-angle*360);
		neck->SetMaxMotorTorque(head->body->GetMass()*750*(std::abs(angle)*5));
		float bodAngle = torso->body->GetAngle();
		torso->body->SetAngularVelocity(-bodAngle*10);
		if(torso->body->GetPosition().y < 0){
			torso->applyLinearImpulseUp(250);
		}
	//}
	
	/*if(angle < neck->GetLowerLimit()/2 || angle > neck->GetUpperLimit()/2){
		neck->SetMotorSpeed(angle < 0 ? 0.1 : -0.1);
		neck->SetMaxMotorTorque(head->body->GetMass()*1000);
	}else{
		neck->SetMotorSpeed(0);
		neck->SetMaxMotorTorque(0);
	}*/

	//if(ai && reactiveFeet){
		b2ContactEdge * stuff = leftLowerLeg->body->GetContactList();
		if(stuff != nullptr && stuff->contact->IsTouching()){
			int type1 = (int)stuff->contact->GetFixtureA()->GetUserData();
			int type2 = (int)stuff->contact->GetFixtureB()->GetUserData();

			if(type1 != type2){
				if(leftLowerLeg->body->GetLinearVelocity().y <= 0.1){
					leftLowerLeg->applyLinearImpulseUp(125);	
					torso->applyLinearImpulseUp(125);	
				}
			}
		}
		stuff = rightLowerLeg->body->GetContactList();
		if(stuff != nullptr && stuff->contact->IsTouching()){
			int type1 = (int)stuff->contact->GetFixtureA()->GetUserData();
			int type2 = (int)stuff->contact->GetFixtureB()->GetUserData();

			if(type1 != type2){
				if(rightLowerLeg->body->GetLinearVelocity().y <= 0.1){
					rightLowerLeg->applyLinearImpulseUp(125);
					torso->applyLinearImpulseUp(125);	
				}
			}
		}
	//}
}

void TestCharacter::attachJoints(){

	/*for(NodeChild * s : children){
		dynamic_cast<Box2DSprite *>(s)->transform->scale(5,5,1);
	}*/
	b2Filter filter;
	filter.groupIndex = -1;
	torso->createFixture(filter);
	head->createFixture(filter);

	// sensor
	b2PolygonShape tShape;
	tShape.SetAsBox(torso->width*std::abs(parents.at(0)->getScaleVector().x)*torso->scale*4.f, std::abs(torso->height*parents.at(0)->getScaleVector().y)*torso->scale*5.f);

	b2Fixture * s = torso->body->CreateFixture(&tShape, 1); // physical fixture
	s->SetSensor(true);

	s->SetFilterData(filter);
	
	float correctedTorsoWidth = torso->getCorrectedWidth();
	float correctedTorsoHeight = torso->getCorrectedHeight();

	// neck
	b2RevoluteJointDef jth;
	jth.bodyA = torso->body;
	jth.bodyB = head->body;
	jth.localAnchorA.Set(ratioX_torso_to_neck * correctedTorsoWidth, ratioY_torso_to_neck * correctedTorsoHeight);
	jth.localAnchorB.Set(ratioX_neck_to_torso * head->getCorrectedWidth(), ratioY_neck_to_torso * -head->getCorrectedHeight());
	jth.collideConnected = false;
	jth.enableLimit = true;
	jth.enableMotor = true;
	jth.maxMotorTorque = 0;
	jth.motorSpeed = 0;
	jth.referenceAngle = 0;
	jth.lowerAngle = -glm::radians(45.f);
	jth.upperAngle = glm::radians(45.f);
	box2dWorld->b2world->CreateJoint(&jth);

	// left shoulder
	leftUpperArm->createFixture(filter);
	b2RevoluteJointDef latj;
	latj.bodyA = torso->body;
	latj.bodyB = leftUpperArm->body;
	latj.localAnchorA.Set(-ratioX_torso_to_shoulder * correctedTorsoWidth, ratioY_torso_to_shoulder * correctedTorsoHeight);
	latj.localAnchorB.Set(ratioX_shoulder_to_torso * leftUpperArm->getCorrectedWidth(), ratioY_shoulder_to_torso * leftUpperArm->getCorrectedHeight());
	latj.collideConnected = false;
	latj.enableLimit = true;
	latj.referenceAngle = glm::radians(0.f);
	latj.lowerAngle = glm::radians(-120.f);
	latj.upperAngle = glm::radians(10.f);
	box2dWorld->b2world->CreateJoint(&latj);
	

	// right shoulder
	rightUpperArm->createFixture(filter);
	b2RevoluteJointDef ratj;
	ratj.bodyA = torso->body;
	ratj.bodyB = rightUpperArm->body;
	ratj.localAnchorA.Set(ratioX_torso_to_shoulder * correctedTorsoWidth, ratioY_torso_to_shoulder * correctedTorsoHeight);
	ratj.localAnchorB.Set(ratioX_shoulder_to_torso * leftUpperArm->getCorrectedWidth(), ratioY_shoulder_to_torso * leftUpperArm->getCorrectedHeight());
	ratj.collideConnected = false;
	ratj.enableLimit = true;
	ratj.referenceAngle = glm::radians(0.f);
	ratj.lowerAngle = glm::radians(-10.f);
	ratj.upperAngle = glm::radians(120.f);
	box2dWorld->b2world->CreateJoint(&ratj);
	
	
	// left elbow
	leftLowerArm->createFixture(filter);
	b2RevoluteJointDef lelsj;
	lelsj.bodyA = leftUpperArm->body;
	lelsj.bodyB = leftLowerArm->body;
	lelsj.localAnchorA.Set(-ratioX_shoulder_to_elbow * leftUpperArm->getCorrectedWidth(), -ratioY_shoulder_to_elbow * leftUpperArm->getCorrectedHeight());
	lelsj.localAnchorB.Set(-ratioX_elbow_to_shoulder * leftLowerArm->getCorrectedWidth(), ratioY_elbow_to_shoulder * leftLowerArm->getCorrectedHeight());
	lelsj.collideConnected = false;
	lelsj.enableLimit = true;
	lelsj.referenceAngle = glm::radians(0.f);
	lelsj.lowerAngle = glm::radians(-10.f);
	lelsj.upperAngle = glm::radians(90.f);
	box2dWorld->b2world->CreateJoint(&lelsj);

	// right elbow
	rightLowerArm->createFixture(filter);
	b2RevoluteJointDef rersj;
	rersj.bodyA = rightUpperArm->body;
	rersj.bodyB = rightLowerArm->body;
	rersj.localAnchorA.Set(-ratioX_shoulder_to_elbow * rightUpperArm->getCorrectedWidth(), -ratioY_shoulder_to_elbow * rightUpperArm->getCorrectedHeight());
	rersj.localAnchorB.Set(ratioX_elbow_to_shoulder * rightLowerArm->getCorrectedWidth(), ratioY_elbow_to_shoulder * rightLowerArm->getCorrectedHeight());
	rersj.collideConnected = false;
	rersj.enableLimit = true;
	rersj.referenceAngle = glm::radians(0.f);
	rersj.lowerAngle = glm::radians(-90.f);
	rersj.upperAngle = glm::radians(10.f);
	box2dWorld->b2world->CreateJoint(&rersj);

	// left hand
	leftHand->createFixture(filter);
	b2RevoluteJointDef lhlej;
	lhlej.bodyA = leftLowerArm->body;
	lhlej.bodyB = leftHand->body;
	lhlej.localAnchorA.Set(-ratioX_elbow_to_wrist * leftLowerArm->getCorrectedWidth(), -ratioY_elbow_to_wrist * leftLowerArm->getCorrectedHeight());
	lhlej.localAnchorB.Set(-ratioX_wrist_to_elbow * leftHand->getCorrectedWidth(), ratioY_wrist_to_elbow * leftHand->getCorrectedHeight());
	lhlej.collideConnected = false;
	lhlej.enableLimit = true;
	lhlej.referenceAngle = glm::radians(0.f);
	lhlej.lowerAngle = glm::radians(-20.f);
	lhlej.upperAngle = glm::radians(20.f);
	box2dWorld->b2world->CreateJoint(&lhlej);

	// right hand
	rightHand->createFixture(filter);
	b2RevoluteJointDef rhrej;
	rhrej.bodyA = rightLowerArm->body;
	rhrej.bodyB = rightHand->body;
	rhrej.localAnchorA.Set(-ratioX_elbow_to_wrist * rightLowerArm->getCorrectedWidth(), -ratioY_elbow_to_wrist * rightLowerArm->getCorrectedHeight());
	rhrej.localAnchorB.Set(ratioX_wrist_to_elbow * rightHand->getCorrectedWidth(), ratioY_wrist_to_elbow * rightHand->getCorrectedHeight());
	rhrej.collideConnected = false;
	rhrej.enableLimit = true;
	rhrej.referenceAngle = glm::radians(0.f);
	rhrej.lowerAngle = glm::radians(-20.f);
	rhrej.upperAngle = glm::radians(20.f);
	box2dWorld->b2world->CreateJoint(&rhrej);

	// left hip
	leftUpperLeg->createFixture(filter);
	b2RevoluteJointDef lltj;
	lltj.bodyA = torso->body;
	lltj.bodyB = leftUpperLeg->body;
	lltj.localAnchorA.Set(-ratioX_torso_to_hip * correctedTorsoWidth, -ratioY_torso_to_hip * correctedTorsoHeight);
	lltj.localAnchorB.Set(ratioX_hip_to_torso * leftUpperLeg->getCorrectedWidth(),  ratioY_hip_to_torso * leftUpperLeg->getCorrectedHeight());
	lltj.collideConnected = false;
	lltj.enableLimit = true;
	lltj.lowerAngle = glm::radians(-100.f);
	lltj.upperAngle = glm::radians(25.f);
	box2dWorld->b2world->CreateJoint(&lltj);
	
	// right hip
	rightUpperLeg->createFixture(filter);
	b2RevoluteJointDef lrtj;
	lrtj.bodyA = torso->body;
	lrtj.bodyB = rightUpperLeg->body;
	lrtj.localAnchorA.Set(ratioX_torso_to_hip * correctedTorsoWidth, -ratioY_torso_to_hip * correctedTorsoHeight);
	lrtj.localAnchorB.Set(ratioX_hip_to_torso * leftUpperLeg->getCorrectedWidth(), ratioY_hip_to_torso * rightUpperLeg->getCorrectedHeight());
	lrtj.collideConnected = false;
	lrtj.enableLimit = true;
	lrtj.lowerAngle = glm::radians(-25.f);
	lrtj.upperAngle = glm::radians(100.f);
	box2dWorld->b2world->CreateJoint(&lrtj);

	// left knee
	leftLowerLeg->createFixture(filter);
	b2RevoluteJointDef llltj;
	llltj.bodyA = leftUpperLeg->body;
	llltj.bodyB = leftLowerLeg->body;
	llltj.localAnchorA.Set(ratioX_hip_to_knee * leftUpperLeg->getCorrectedWidth(), -ratioY_hip_to_knee * leftUpperLeg->getCorrectedHeight());
	llltj.localAnchorB.Set(ratioX_knee_to_hip * leftLowerLeg->getCorrectedWidth(), ratioY_knee_to_hip * leftLowerLeg->getCorrectedHeight());
	llltj.collideConnected = false;
	llltj.enableLimit = true;
	llltj.lowerAngle = glm::radians(0.f);
	llltj.upperAngle = glm::radians(75.f);
	box2dWorld->b2world->CreateJoint(&llltj);
	
	// right knee
	rightLowerLeg->createFixture(filter);
	b2RevoluteJointDef lrltj;
	lrltj.bodyA = rightUpperLeg->body;
	lrltj.bodyB = rightLowerLeg->body;
	lrltj.localAnchorA.Set(-ratioX_hip_to_knee * rightUpperLeg->getCorrectedWidth(), -ratioY_hip_to_knee * rightUpperLeg->getCorrectedHeight());
	lrltj.localAnchorB.Set(-ratioX_knee_to_hip * rightLowerLeg->getCorrectedWidth(), ratioY_knee_to_hip * rightLowerLeg->getCorrectedHeight());
	lrltj.collideConnected = false;
	lrltj.enableLimit = true;
	lrltj.lowerAngle = glm::radians(-75.f);
	lrltj.upperAngle = glm::radians(0.f);
	box2dWorld->b2world->CreateJoint(&lrltj);

	torso->body->SetLinearDamping(0.9);
	torso->body->SetGravityScale(0.5);
	
	for(Box2DSprite ** c : components){
		childTransform->addChild(*c);
		(*c)->unload();
		(*c)->load();
	}
}


void TestCharacter::setShader(Shader * _shader, bool _configureDefaultVertexAttributes){
	MeshEntity::setShader(_shader, _configureDefaultVertexAttributes);
	for(Box2DSprite ** c : components){
		if(*c != nullptr){
			(*c)->setShader(_shader, _configureDefaultVertexAttributes);
		}
	}
}

void TestCharacter::translateComponents(glm::vec3 _translateVector){
	for(Box2DSprite ** c : components){
		if(*c != nullptr){
			(*c)->setTranslationPhysical(_translateVector, true);
		}
	}
}
