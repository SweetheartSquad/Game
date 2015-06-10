#pragma once

#include <BulletMeshEntity.h>
#include <Box2D/Box2D.h>

class BitmapFont;
class Scene;
class Shader;
class Box2DWorld;
class Box2DSprite;
class BulletWorld;
class Texture;

class ComponentTexture{
public:
	Texture * texture;
	float width;
	float height;

	ComponentTexture(Texture * _texture, float _width, float _height);
};

class Character : public BulletMeshEntity{
public:
	float ratioX_neck_to_torso;
	float ratioY_neck_to_torso;
	float ratioX_torso_to_neck;
	float ratioY_torso_to_neck;

	float ratioX_shoulder_to_torso;
	float ratioY_shoulder_to_torso;
	float ratioX_torso_to_shoulder;
	float ratioY_torso_to_shoulder;
	
	float ratioX_elbow_to_shoulder;
	float ratioY_elbow_to_shoulder;
	float ratioX_shoulder_to_elbow;
	float ratioY_shoulder_to_elbow;

	float ratioX_wrist_to_elbow;
	float ratioY_wrist_to_elbow;
	float ratioX_elbow_to_wrist;
	float ratioY_elbow_to_wrist;
	
	float ratioX_hip_to_torso;
	float ratioY_hip_to_torso;
	float ratioX_torso_to_hip;
	float ratioY_torso_to_hip;
	
	float ratioX_knee_to_hip;
	float ratioY_knee_to_hip;
	float ratioX_hip_to_knee;
	float ratioY_hip_to_knee;

	float hipWidth;

	Box2DWorld * box2dWorld;

	Box2DSprite * torso;
	Box2DSprite * head;
	Box2DSprite * leftUpperArm;
	Box2DSprite * leftLowerArm;
	Box2DSprite * leftHand;
	Box2DSprite * rightUpperArm;
	Box2DSprite * rightLowerArm;
	Box2DSprite * rightHand;
	Box2DSprite * leftUpperLeg;
	Box2DSprite * leftLowerLeg;
	Box2DSprite * rightUpperLeg;
	Box2DSprite * rightLowerLeg;

	std::vector<Box2DSprite **> components;

	explicit Character(BulletWorld * _world);
	
	~Character();

	void render(vox::MatrixStack* _matrixStack, RenderOptions* _renderStack) override;
	void update(Step* _step) override;
	void attachJoints();

	void setShader(Shader * _shader, bool _configureDefaultVertexAttributes) override;

	void translateComponents(glm::vec3 _translateVector);
};