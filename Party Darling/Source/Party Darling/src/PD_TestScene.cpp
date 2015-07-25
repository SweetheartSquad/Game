#pragma once

#include <PD_TestScene.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>
#include <PD_Button.h>
#include <PD_TalkToButton.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentHsv.h>
#include <shader\ShaderComponentMVP.h>

#include <shader\ShaderComponentIndexedTexture.h>
#include <TextureColourTable.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDrawer.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>

#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <TextArea.h>

#include <Room.h>
#include <RoomBuilder.h>


#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
// Retrieves a JSON value from an HTTP request.
pplx::task<void> RequestJSONValueAsync(){
	// TODO: To successfully use this example, you must perform the request  
	// against a server that provides JSON data.
	web::http::client::http_client client(L"https://seniorproject-ryanbluth.c9.io/api/users");
	return client.request(web::http::methods::GET).then([](web::http::http_response response) -> pplx::task<web::json::value>{
		std::wcout << L"Response recieved" << std::endl << L"Status: " << response.status_code() << std::endl;
		if(response.status_code() == web::http::status_codes::OK){
			auto json = response.extract_json();
			std::wostringstream ss;
			ss << json.get()[0].at(L"user").at(L"email").as_string() << std::endl;
			std::wcout << ss.str();
            return json;
		}else{
			std::wcout << L"No response because the code wasn't ok." << std::endl;
		}

		// Handle error cases, for now return empty json value... 
		return pplx::task_from_result(web::json::value());
	}).then([](pplx::task<web::json::value> previousTask){
		try{
			const web::json::value& v = previousTask.get();
			// Perform actions here to process the JSON value...
		}catch (const web::http::http_exception& e){
			// Print error.
			std::wostringstream ss;
			ss << e.what() << std::endl;
			std::wcout << ss.str();
		}
	});
}









PD_TestScene::PD_TestScene(Game * _game) :
	Scene(_game),
	textShader(new ComponentShaderText(true)),
	characterShader(new ComponentShaderBase(true)),
	diffuseShader(new ComponentShaderBase(true)),
	debugDrawer(nullptr),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	uiLayer(this, 0,0,0,0)
{
	characterShader->addComponent(new ShaderComponentMVP(characterShader));
	characterShader->addComponent(new ShaderComponentIndexedTexture(characterShader));
	characterShader->compileShader();
	
	diffuseShader->addComponent(new ShaderComponentMVP(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentTexture(diffuseShader));
	diffuseShader->addComponent(new ShaderComponentDiffuse(diffuseShader));
	diffuseShader->compileShader();

	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));

	//Set up debug camera
	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	childTransform->addChild(debugCam, false);
	debugCam->farClip = 1000.f;
	debugCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;


	bulletWorld = new BulletWorld();

	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh());
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->setShader(diffuseShader, true);
	bulletGround->mesh->parents.at(0)->scale(1000,1000,1000);
	bulletGround->mesh->parents.at(0)->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, -1, 0));
	bulletGround->body->setFriction(1);

	ComponentShaderBase * backgroundShader = new ComponentShaderBase(true);
	backgroundShader->addComponent(new ShaderComponentIndexedTexture(backgroundShader));
	backgroundShader->compileShader();
	
	//label = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::WORD_WRAP, 300);
	//label->setText(L"userId");	
	//ragdoll->head->childTransform->addChild(label);
	//label->parents.at(0)->scale(0.01,0.01,0.01);
	//label->parents.at(0)->rotate(90, 1, 0, 0, kOBJECT);
	//label->parents.at(0)->translate(0,5,0);
	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));


	/*NodeUI * uiThing = new NodeUI(bulletWorld, this);
	MeshEntity * uiThingMesh = new MeshEntity(MeshFactory::getCubeMesh());
	uiThing->childTransform->addChild(uiThingMesh);
	uiThing->setColliderAsBox();
	uiThing->createRigidBody(0);
	uiThingMesh->setShader(shader, true);
	childTransform->addChild(uiThing);*/
	
	//Room * room = new Room(bulletWorld, diffuseShader, RoomLayout_t::kRECT, glm::vec2(3.f, 3.f), PD_ResourceManager::scenario->getTexture("UV-TEST-ALT")->texture);
	Room * room = RoomBuilder::getRoom("{}",bulletWorld);
	childTransform->addChild(room);
	room->setShader(diffuseShader, true);
	
	std::vector<std::string> objs;
	objs.push_back("assets/meshes/LOD_2/coffeeTable_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/couch_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/dish_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/dresser_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/lamp_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/shelf_LOD_2.obj");
	objs.push_back("assets/meshes/LOD_2/vase_LOD_2.obj");
	std::vector<std::string> staticobjs;
	staticobjs.push_back("assets/meshes/LOD_2/door_LOD_2.obj");
	//staticobjs.push_back("assets/LOD_1/roomBox_LOD_1.obj"); // we need to make separate pieces for the walls/ground otherwise it wont collide properly
	staticobjs.push_back("assets/meshes/LOD_2/windowFrame_LOD_2.obj");
	/*
	for(std::string s : objs){
		BulletMeshEntity * obj = new BulletMeshEntity(bulletWorld, Resource::loadMeshFromObj(s).at(0));
		obj->setColliderAsBoundingBox();
		obj->createRigidBody(25);
		obj->setShader(diffuseShader, true);
		childTransform->addChild(obj);
	}
	*/
	for(std::string s : staticobjs){
		BulletMeshEntity * obj = new BulletMeshEntity(bulletWorld, Resource::loadMeshFromObj(s).at(0));
		obj->setColliderAsMesh(Resource::loadMeshFromObj(s).at(0), false);
		obj->createRigidBody(0);
		obj->setShader(diffuseShader, true);
		childTransform->addChild(obj);
	}
	

	glm::uvec2 sd = vox::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);

	// mouse cursor
	mouseIndicator = new Sprite();
	uiLayer.childTransform->addChild(mouseIndicator);
	mouseIndicator->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CURSOR")->texture);
	mouseIndicator->parents.at(0)->scale(32, 32, 1);
	mouseIndicator->mesh->scaleModeMag = GL_NEAREST;
	mouseIndicator->mesh->scaleModeMin = GL_NEAREST;

	for(unsigned long int i = 0; i < mouseIndicator->mesh->vertices.size(); ++i){
		mouseIndicator->mesh->vertices[i].x += 0.5f;
		mouseIndicator->mesh->vertices[i].y -= 0.5f;
	}
	mouseIndicator->mesh->dirty = true;
	mouseIndicator->setShader(uiLayer.shader, true);


	// crosshair
	crosshair = new Sprite();
	uiLayer.childTransform->addChild(crosshair);
	crosshair->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshair->parents.at(0)->scale(16, 16, 1);
	crosshair->setShader(uiLayer.shader, true);
	crosshair->mesh->scaleModeMag = GL_NEAREST;
	crosshair->mesh->scaleModeMin = GL_NEAREST;




	// player set-up
	playerEntity = new BulletMeshEntity(bulletWorld, MeshFactory::getCubeMesh());
	childTransform->addChild(playerEntity, true);
	playerEntity->parents.at(0)->scale(1, 1.5f, 1);
	playerEntity->parents.at(0)->scale(2);
	playerEntity->setColliderAsCapsule(1, 1.5f);
	playerEntity->createRigidBody(1);
	playerEntity->setShader(diffuseShader, true);
	playerEntity->body->setDamping(0.1, 0.1);
	playerEntity->body->setAngularFactor(btVector3(0,1,0));

	playerCam = new MousePerspectiveCamera();
	playerEntity->childTransform->addChild(playerCam);
	cameras.push_back(playerCam);
	playerCam->farClip = 1000.f;
	playerCam->nearClip = 0.1f;
	playerCam->childTransform->rotate(90, 0, 1, 0, kWORLD);
	playerCam->parents.at(0)->translate(0, 1, 0);
	playerCam->yaw = 90.0f;
	playerCam->pitch = -10.0f;
	playerCam->speed = 1;
	activeCamera = playerCam;
	
	PointLight * light2 = new PointLight(glm::vec3(1,1,1), 0.02f, 0.001f, -1);
	lights.push_back(light2);
	playerCam->childTransform->addChild(light2);

	playerController = new PD_FirstPersonController(playerEntity, playerCam);
	childTransform->addChild(playerController, false);


	

	// 3D ui testing stuff
	PD_TalkToButton * butt1 = new PD_TalkToButton(PD_ResourceManager::scenario->conversations["test1"], bulletWorld, this);
	childTransform->addChild(butt1);
	butt1->setTranslationPhysical(20, 20, 0, true);
	
	butt1->body->getWorldTransform().getOrigin().rotate(btVector3(0,1,0), 45);
	//butt1->body->setWorldTransform();45, 0, 1, 0, kOBJECT);

	PD_TalkToButton * butt2 = new PD_TalkToButton(PD_ResourceManager::scenario->conversations["test2"], bulletWorld, this);
	childTransform->addChild(butt2);
	butt2->setTranslationPhysical(20, 30, -23, true);
	butt2->childTransform->rotate(45, 1, 1, 0, kOBJECT);

	PD_TalkToButton * butt4 = new PD_TalkToButton(PD_ResourceManager::scenario->conversations["test2"], bulletWorld, this);
	childTransform->addChild(butt4);
	butt4->setTranslationPhysical(20, 40, -23, true);
	butt4->parents.at(0)->rotate(45, 1, 1, 0, kOBJECT);

	
	TextArea * butt3 = new TextArea(uiLayer.world, this, PD_ResourceManager::scenario->getFont("DEFAULT")->font, textShader, 100.f);
	butt3->setMouseEnabled(true);
	uiLayer.addChild(butt3);
	butt3->setText(L"exit");
	butt3->onClickFunction = [this](NodeUI * _this){
		game->exit();
	};
	butt3->setTranslationPhysical(100, 0, 0, true);
	butt3->setBackgroundColour(1,1,1,1);






	// palette testing stuff
	playerPalette = new TextureColourTable(false);
	playerPalette->load();
	PD_ResourceManager::resources.push_back(playerPalette);
	
	{
	Sprite * testSprite = new Sprite();
	testSprite->setShader(characterShader, true);
	childTransform->addChild(testSprite);
	testSprite->parents.at(0)->scale(10);
	testSprite->parents.at(0)->translate(6, 4, -6);
	testSprite->mesh->pushTexture2D(playerPalette);
	testSprite->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("INDEXED-TEST2")->texture);
	testSprite->mesh->scaleModeMag = GL_NEAREST;
	testSprite->mesh->scaleModeMin = GL_NEAREST;
	}
	{
	Sprite * testSprite = new Sprite();
	testSprite->setShader(characterShader, true);
	childTransform->addChild(testSprite);
	testSprite->parents.at(0)->scale(10);
	testSprite->parents.at(0)->translate(-3, 3, -3);
	testSprite->mesh->pushTexture2D(playerPalette);
	testSprite->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("INDEXED-TEST")->texture);
	testSprite->mesh->scaleModeMag = GL_NEAREST;
	testSprite->mesh->scaleModeMin = GL_NEAREST;
	}
}

PD_TestScene::~PD_TestScene(){
	deleteChildTransform();
	diffuseShader->safeDelete();
	characterShader->safeDelete();
	textShader->safeDelete();

	screenSurface->safeDelete();
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
	delete bulletWorld;
}


void PD_TestScene::update(Step * _step){
	glm::vec3 curpos = activeCamera->getWorldPos();
	NodeOpenAL::setListenerVelocity((curpos - lastPos));
	lastPos = curpos;

	NodeOpenAL::setListenerPosition(activeCamera->getWorldPos());
	NodeOpenAL::setListenerOrientation(activeCamera->forwardVectorRotated, activeCamera->upVectorRotated);
	
	if(keyboard->keyDown(GLFW_KEY_P) || keyboard->keyJustDown(GLFW_KEY_O)){
		playerPalette->generateRandomTable();
		playerPalette->bufferData();
	}
	if(keyboard->keyJustDown(GLFW_KEY_I)){
		playerPalette->saveImageData("palette.tga");
		PD_ResourceManager::scenario->getTexture("INDEXED-TEST")->texture->saveImageData("INDEXED-TEST.tga");
		PD_ResourceManager::scenario->getTexture("INDEXED-TEST2")->texture->saveImageData("INDEXED-TEST2.tga");
	}
	
	if(keyboard->keyJustUp(GLFW_KEY_E)){	
		std::wcout << L"Calling RequestJSONValueAsync..." << std::endl;
		//RequestJSONValueAsync(label);
	}
	if(keyboard->keyJustUp(GLFW_KEY_R)){	
		std::stringstream sql;
		sql << "DROP TABLE IF EXISTS TestTable;";
		sql << "CREATE TABLE TestTable(id INTEGER PRIMARY KEY, TestColumn1, TestColumn2);";
		for(unsigned long int i = 0; i < 1000; ++i){
			sql << "INSERT INTO TestTable VALUES(" << i << ", 'test1', 'test2');";
		}
		sql << "SELECT * FROM TestTable;";
		PD_ResourceManager::testSql(sql.str(), true);
	}

	float speed = 1;
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
	if (keyboard->keyDown(GLFW_KEY_F11)){
		screenFBO->saveToFile("fboTest.tga", 0);
	}

	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_1)){
		cycleCamera();
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
		if(debugDrawer != nullptr){
			bulletWorld->world->setDebugDrawer(nullptr);
			childTransform->removeChild(debugDrawer);
			delete debugDrawer;
			debugDrawer = nullptr;
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_NoDebug);
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld->world);
			childTransform->addChild(debugDrawer, false);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			bulletWorld->world->setDebugDrawer(debugDrawer);
			uiLayer.bulletDebugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
		}
	}
	
	// update scene and physics
	bulletWorld->update(_step);
	Scene::update(_step);

	// update ui stuff
	glm::uvec2 sd = vox::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	glm::vec3 sp = activeCamera->worldToScreen(glm::vec3(0,0,0), sd);
	if(sp.z < 0){
		sp.z = activeCamera->farClip * 2;
	}
	mouseIndicator->parents.at(0)->translate(mouse->mouseX(), mouse->mouseY(), 0, false);
	crosshair->parents.at(0)->translate(sd.x*0.5f, sd.y*0.5f, 0, false);
}

void PD_TestScene::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	clear();
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	Scene::render(_matrixStack, _renderOptions);
	//Render the buffer to the render surface
	screenSurface->render(screenFBO->getTextureId());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	uiLayer.render(_matrixStack, _renderOptions);
}

void PD_TestScene::load(){
	Scene::load();	

	screenSurface->load();
	screenFBO->load();
	uiLayer.load();
}

void PD_TestScene::unload(){
	uiLayer.unload();
	screenFBO->unload();
	screenSurface->unload();

	Scene::unload();	
}