#include <PD_Scene_FurnitureTest.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>

#include <NumberUtils.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>

#include <RenderOptions.h>
#include <json\json.h>

#include <sweet/Input.h>

PD_Scene_FurnitureTest::PD_Scene_FurnitureTest(Game * _game) :
	Scene(_game),
	uiLayer(0,0,0,0),
	shader(new ComponentShaderBase(false))
{
	shader->addComponent(new ShaderComponentMVP(shader));
	shader->addComponent(new ShaderComponentTexture(shader));
	//shader->addComponent(new ShaderComponentDiffuse(shader));
	shader->compileShader();

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);

	mouseIndicator = new Sprite(uiLayer.shader);

	uiLayer.childTransform->addChild(mouseIndicator);
	mouseIndicator->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CURSOR")->texture);
	mouseIndicator->parents.at(0)->scale(32,32,1);
	mouseIndicator->mesh->scaleModeMag = GL_NEAREST;
	mouseIndicator->mesh->scaleModeMin = GL_NEAREST;

	for(unsigned long int i = 0; i < mouseIndicator->mesh->vertices.size(); ++i){
		mouseIndicator->mesh->vertices[i].x += 0.5f;
		mouseIndicator->mesh->vertices[i].y -= 0.5f;
	}
	mouseIndicator->mesh->dirty = true;

	MeshInterface * lamp = Resource::loadMeshFromObj("assets/meshes/LOD_2/lamp_LOD_2.obj").at(0);
	lamp->pushTexture2D(PD_ResourceManager::scenario->getTexture("DEFAULT")->texture);
	MeshEntity * meshEntity = new MeshEntity(lamp, shader);

	childTransform->addChild(meshEntity);

	Transform * furniture1 = new Transform();

	furniture1 = createChair();

	Transform * furniture2 = new Transform();

	furniture2 = createChair();

	childTransform->addChild(furniture1);
	childTransform->addChild(furniture2);
	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	//Set up debug camera
	MousePerspectiveCamera * debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;
	activeCamera = debugCam;
}

PD_Scene_FurnitureTest::~PD_Scene_FurnitureTest(){
	deleteChildTransform();
}

void PD_Scene_FurnitureTest::update(Step * _step){
	if(keyboard->keyJustDown(GLFW_KEY_F12)){
		game->toggleFullScreen();
	}

	if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
	}float speed = 1;
	MousePerspectiveCamera * cam = dynamic_cast<MousePerspectiveCamera *>(activeCamera);
	if(cam != nullptr){
		speed = cam->speed;
	}
	// camera controls
	if (keyboard->keyDown(GLFW_KEY_UP)){
		activeCamera->parents.at(0)->translate((activeCamera->forwardVectorRotated) * speed);
	}
	if (keyboard->keyDown(GLFW_KEY_DOWN)){
		activeCamera->parents.at(0)->translate((activeCamera->forwardVectorRotated) * -speed);
	}
	if (keyboard->keyDown(GLFW_KEY_LEFT)){
		activeCamera->parents.at(0)->translate((activeCamera->rightVectorRotated) * -speed);
	}
	if (keyboard->keyDown(GLFW_KEY_RIGHT)){
		activeCamera->parents.at(0)->translate((activeCamera->rightVectorRotated) * speed);
	}

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);

	mouseIndicator->parents.at(0)->translate(mouse->mouseX(), mouse->mouseY(), 0, false);

	

}

void PD_Scene_FurnitureTest::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_Scene_FurnitureTest::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_FurnitureTest::unload(){
	uiLayer.unload();
	Scene::unload();	
}

Transform * PD_Scene_FurnitureTest::createChair(){
	int textureID = sweet::NumberUtils::randomInt(1,2);

	std::string chairTexture = "DEFAULT";

	if (textureID == 1){
		//leave texture as default
	}

	if (textureID == 2){
		chairTexture = "RYAN";
	}
	MeshInterface * chairBack = Resource::loadMeshFromObj("assets/TestChairComponents/Chair_Back_1.obj").at(0);
	chairBack->pushTexture2D(PD_ResourceManager::scenario->getTexture(chairTexture)->texture);
	MeshEntity * chairBack_Entity = new MeshEntity(chairBack, shader);

	MeshInterface * chairBase = Resource::loadMeshFromObj("assets/TestChairComponents/Chair_Base_1.obj").at(0);
	chairBase->pushTexture2D(PD_ResourceManager::scenario->getTexture(chairTexture)->texture);
	MeshEntity * chairBase_Entity = new MeshEntity(chairBase, shader);

	MeshInterface * chairLegs = Resource::loadMeshFromObj("assets/TestChairComponents/Chair_Legs_1.obj").at(0);
	chairLegs->pushTexture2D(PD_ResourceManager::scenario->getTexture(chairTexture)->texture);
	MeshEntity * chairLegs_Entity = new MeshEntity(chairLegs, shader);

	Transform * chairTransform = new Transform();
	chairTransform ->addChild(chairBack_Entity,false);
	chairTransform ->addChild(chairBase_Entity,false);
	chairTransform ->addChild(chairLegs_Entity,false);


	Json::Value root;
	int id;

	Json::Reader reader;
	std::string jsonLoaded = FileUtils::readFile("assets/furniture.json");
	bool parsingSuccessful = reader.parse( jsonLoaded, root );
	if(!parsingSuccessful){
		Log::error("JSON parse failed: " + reader.getFormattedErrorMessages()/* + "\n" + jsonLoaded*/);
	}else{
		 Json::Value furnitureJson = root["furniture"];

		 for(Json::Value v : furnitureJson){
			std::cout << v; 
		 }
	}



	int xValue = sweet::NumberUtils::randomInt(0,50);
	int zValue = sweet::NumberUtils::randomInt(0,50);

	chairTransform ->translate(xValue,0,zValue);
	return chairTransform;

}
