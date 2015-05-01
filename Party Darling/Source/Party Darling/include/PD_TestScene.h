#pragma once


#include <ft2build.h>
#include FT_FREETYPE_H

#include <Scene.h>
#include <SoundManager.h>

#include <Text.h>
#include <OrthographicCamera.h>

class MousePerspectiveCamera;
class FollowCamera;

class Box2DWorld;
class Box2DDebugDraw;
class Box2DMeshEntity;
class MeshEntity;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;

class PD_Player;

class PD_TestScene : public Scene{
public:
	BaseComponentShader * shader;
	ShaderComponentHsv * hsvComponent;
	MousePerspectiveCamera * mouseCam;
	MousePerspectiveCamera * debugCam;
	PerspectiveCamera * gameCam;

	MeshEntity * ground;

	Box2DWorld * world;
	Box2DDebugDraw * drawer;
	PD_Player * player;

	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	Material * phongMat;

	FT_Library ft_lib;

	Text * text;
	
	float sceneHeight;
	float sceneWidth;

	bool firstPerson;

	virtual void update(Step * _step) override;
	virtual void render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	void render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy);

	virtual void load() override;
	virtual void unload() override;

	std::vector<MeshEntity *> audioVisualizer;

	PD_TestScene(Game * _game);
	~PD_TestScene();
};