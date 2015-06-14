#pragma once

#include <PD_TestScene.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <Material.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentPhong.h>
#include <shader\ShaderComponentBlinn.h>
#include <shader\ShaderComponentShadow.h>
#include <shader\ShaderComponentHsv.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDrawer.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>
#include <MatrixStack.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>
#include <RenderOptions.h>
#include <shader\ShaderComponentText.h>
#include <StringUtils.h>
#include <CharacterUtils.h>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <Billboard.h>

#include <BulletRagdoll.h>
#include <NodeUI.h>
#include <PD_Button.h>

#include <OpenALSound.h>
#include <sqlite\sqlite3.h>
#include <DatabaseConnection.h>

#include <Room.h>
#include <RoomLayout.h>

#include <thread>
#include <Character.h>
#include <LinearLayout.h>
#include <shader\ComponentShaderText.h>

// Retrieves a JSON value from an HTTP request.
pplx::task<void> RequestJSONValueAsync(Label * _label){
	// TODO: To successfully use this example, you must perform the request  
	// against a server that provides JSON data.
	web::http::client::http_client client(L"https://seniorproject-ryanbluth.c9.io/api/users");
	return client.request(web::http::methods::GET).then([_label](web::http::http_response response) -> pplx::task<web::json::value>{
		std::wcout << L"Response recieved" << std::endl << L"Status: " << response.status_code() << std::endl;
		if(response.status_code() == web::http::status_codes::OK){
			auto json = response.extract_json();

			_label->setText(json.get()[0].at(L"user").at(L"email").as_string());
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




static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

DatabaseConnection db("../assets/test.db");
void testSql(std::string _sql, bool _async){
	if(_async){
		db.queryDbAsync(_sql, callback);
	}else{
		db.queryDb(_sql, callback);
	}
}




PD_TestScene::PD_TestScene(Game * _game) :
	Scene(_game),
	shader(new ComponentShaderBase(true)),
	textShader(new ComponentShaderText(true)),
	hsvComponent(new ShaderComponentHsv(shader, 0, 1, 1)),
	debugDrawer(nullptr),
	screenSurfaceShader(new Shader("../assets/RenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	phongMat(new Material(15.0, glm::vec3(1.f, 1.f, 1.f), true)),
	sceneHeight(150),
	sceneWidth(50),
	firstPerson(true),
	joy(new JoystickManager()),
	uiLayer(0,0,0,0)
{

	shader->addComponent(new ShaderComponentTexture(shader));
	shader->addComponent(new ShaderComponentDiffuse(shader));
	shader->addComponent(hsvComponent);
	//shader->addComponent(new ShaderComponentPhong(shader));
	//shader->addComponent(new ShaderComponentBlinn(shader));
	//shader->addComponent(new ShaderComponentShadow(shader));

	shader->compileShader();


	//Set up cameras
	Transform * t = new Transform();
	mouseCam = new MousePerspectiveCamera();
	t->addChild(mouseCam, false);
	cameras.push_back(mouseCam);
	mouseCam->farClip = 1000.f;
	mouseCam->nearClip = 0.1f;
	mouseCam->parents.at(0)->rotate(90, 0, 1, 0, kWORLD);
	mouseCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	mouseCam->yaw = 90.0f;
	mouseCam->pitch = -10.0f;
	mouseCam->speed = 1;

	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	t = new Transform();
	t->addChild(debugCam, false);
	debugCam->farClip = 1000.f;
	debugCam->parents.at(0)->rotate(90, 0, 1, 0, kWORLD);
	debugCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;

	gameCam = new FollowCamera(15, glm::vec3(0, 0, 0), 0, 0);
	cameras.push_back(gameCam);
	t = new Transform();
	t->addChild(gameCam, false);
	gameCam->farClip = 1000.f;
	gameCam->parents.at(0)->rotate(90, 0, 1, 0, kWORLD);
	gameCam->parents.at(0)->translate(5.0f, 1.5f, 22.5f);
	gameCam->minimumZoom = 22.5f;
	gameCam->yaw = 90.0f;
	gameCam->pitch = -10.0f;

	activeCamera = mouseCam;

	/*MeshEntity * ceiling = new MeshEntity(MeshFactory::getPlaneMesh());
	ceiling->transform->translate(sceneWidth/2.f, sceneHeight/2.f, _size * 4.f);
	ceiling->transform->scale(sceneWidth, sceneHeight, 1);
	ceiling->setShader(shader, true);
	addChild(ceiling);*/

	/*Billboard * billboard = new Billboard();
	billboard->mesh->pushTexture2D(PD_ResourceManager::crosshair);
	billboard->setShader(shader, true);
	player->childTransform->addChild(billboard);
	billboard->parents.at(0)->rotate(90, 1, 0, 0, kOBJECT);
	billboard->parents.at(0)->translate(0, 0, 2);*/

	/*mouseCam->upVectorLocal = glm::vec3(0, 0, 1);
	mouseCam->forwardVectorLocal = glm::vec3(1, 0, 0);
	mouseCam->rightVectorLocal = glm::vec3(0, -1, 0);*/

	crosshair = new Sprite();
	uiLayer.childTransform->addChild(crosshair);
	crosshair->mesh->pushTexture2D(PD_ResourceManager::crosshair);
	crosshair->parents.at(0)->scale(8,8,1);
	crosshair->setShader(uiLayer.shader, true);

	playerIndicator = new Sprite();
	uiLayer.childTransform->addChild(playerIndicator);
	playerIndicator->mesh->pushTexture2D(PD_ResourceManager::crosshair);
	playerIndicator->parents.at(0)->scale(8,8,1);
	playerIndicator->setShader(uiLayer.shader, true);

	mouseIndicator = new Sprite();
	uiLayer.childTransform->addChild(mouseIndicator);
	mouseIndicator->mesh->pushTexture2D(PD_ResourceManager::cursor);
	mouseIndicator->parents.at(0)->scale(32 * 10 * 0.5, 32 * 10 * 0.5, 1);
	mouseIndicator->mesh->scaleModeMag = GL_NEAREST;
	mouseIndicator->mesh->scaleModeMin = GL_NEAREST;

	for(unsigned long int i = 0; i < mouseIndicator->mesh->vertices.size(); ++i){
		mouseIndicator->mesh->vertices[i].x -= 1;
		mouseIndicator->mesh->vertices[i].y -= 1;
	}
	mouseIndicator->mesh->dirty = true;
	mouseIndicator->setShader(uiLayer.shader, true);

	volumeIndicator = new Sprite();
	uiLayer.childTransform->addChild(volumeIndicator);
	volumeIndicator->mesh->pushTexture2D(PD_ResourceManager::cheryl);
	volumeIndicator->parents.at(0)->scale(50, 50, 1);

	for(unsigned long int i = 0; i < volumeIndicator->mesh->vertices.size(); ++i){
		volumeIndicator->mesh->vertices[i].x -= 1;
		volumeIndicator->mesh->vertices[i].y -= 1;
	}
	volumeIndicator->mesh->dirty = true;
	volumeIndicator->setShader(uiLayer.shader, true);

	screenSurface->scaleModeMag = GL_NEAREST;
	screenSurface->scaleModeMin = GL_NEAREST;

	bulletWorld = new BulletWorld();

	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh());
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->setShader(shader, true);
	bulletGround->mesh->parents.at(0)->scale(1000,1000,1000);
	bulletGround->mesh->parents.at(0)->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, -1, 0));
	bulletGround->body->setFriction(10);

	ComponentShaderBase * backgroundShader = new ComponentShaderBase(true);
	backgroundShader->addComponent(new ShaderComponentTexture(backgroundShader));
	backgroundShader->compileShader();

	font = new Font("../assets/arial.ttf", 30, false);
	
	//label->setText(L"userId");	
	//player->childTransform->addChild(label);
	//label->parents.at(0)->scale(0.01,0.01,0.01);
	//label->parents.at(0)->rotate(90, 1, 0, 0, kOBJECT);
	//label->parents.at(0)->translate(0,5,0);
	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));

	/*for(unsigned long int i = 0; i < 1000; ++i){
		MeshEntity * me = new MeshEntity(MeshFactory::getCubeMesh());
		me->setShader(shader, true);
		childTransform->addChild(me);
	}*/
	
	ragdoll = new BulletRagdoll(bulletWorld, 1.0f);
	childTransform->addChild(ragdoll);
	ragdoll->setShader(shader, true);
	ragdoll->head->childTransform->addChild(PD_ResourceManager::stream, false);

	//ragdoll->body->body->setAngularFactor(btVector3(0,0,0)); // keeps body upright
	PointLight * light2 = new PointLight(glm::vec3(1,1,1), 0.02f, 0.001f, -1);
	lights.push_back(light2);
	ragdoll->upperbody->childTransform->addChild(light2);

	font = new Font("../assets/arial.ttf", 30, false);
	label = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::WORD_WRAP, 300);
	label->setText(L"userId");	
	ragdoll->head->childTransform->addChild(label);
	label->parents.at(0)->scale(0.01,0.01,0.01);
	label->parents.at(0)->rotate(90, 1, 0, 0, kOBJECT);
	label->parents.at(0)->translate(0,5,0);
	textShader->textComponent->setColor(glm::vec3(0.0f, 0.0f, 0.0f));


	/*NodeUI * uiThing = new NodeUI(bulletWorld, this);
	MeshEntity * uiThingMesh = new MeshEntity(MeshFactory::getCubeMesh());
	uiThing->childTransform->addChild(uiThingMesh);
	uiThing->setColliderAsBox();
	uiThing->createRigidBody(0);
	uiThingMesh->setShader(shader, true);
	childTransform->addChild(uiThing);*/
	
	Room * room = new Room(bulletWorld, shader, RoomLayout_t::RECT, glm::vec2(3.f, 3.f), PD_ResourceManager::uvs_alt);

	//MeshEntity * room = new MeshEntity(RoomLayout::getWalls(RoomLayout_t::RECT, glm::vec2(3.f, 3.f)));
	childTransform->addChild(room);
	//room->setShader(shader, true);
	//room->mesh->pushMaterial(phongMat);
	//room->mesh->pushTexture2D(PD_ResourceManager::uvs_alt);
	room->setShader(shader, true);
	//room->parents.at(0)->translate(0, ROOM_HEIGHT / 2.f - (1 - 0.05), 0);
	room->translatePhysical(glm::vec3(0, ROOM_HEIGHT / 2.f - (1 - 0.05), 0));
	/*
	MeshEntity * blah = new MeshEntity(MeshFactory::getCubeMesh(), shader);
	childTransform->addChild(blah);
	blah->setShader(shader, true);
	blah->mesh->pushMaterial(phongMat);
	*/
	std::vector<std::string> objs;
	objs.push_back("../assets/LOD_2/coffeeTable_LOD_2.obj");
	objs.push_back("../assets/LOD_2/couch_LOD_2.obj");
	objs.push_back("../assets/LOD_2/dish_LOD_2.obj");
	objs.push_back("../assets/LOD_2/dresser_LOD_2.obj");
	objs.push_back("../assets/LOD_2/lamp_LOD_2.obj");
	objs.push_back("../assets/LOD_2/shelf_LOD_2.obj");
	objs.push_back("../assets/LOD_2/vase_LOD_2.obj");
	std::vector<std::string> staticobjs;
	staticobjs.push_back("../assets/LOD_2/door_LOD_2.obj");
	//staticobjs.push_back("../assets/LOD_1/roomBox_LOD_1.obj"); // we need to make separate pieces for the walls/ground otherwise it wont collide properly
	staticobjs.push_back("../assets/LOD_2/windowFrame_LOD_2.obj");
	for(std::string s : objs){
		BulletMeshEntity * obj = new BulletMeshEntity(bulletWorld, Resource::loadMeshFromObj(s).at(0));
		obj->setColliderAsBoundingBox();
		obj->createRigidBody(25);
		obj->setShader(shader, true);
		childTransform->addChild(obj);
	}
	for(std::string s : staticobjs){
		BulletMeshEntity * obj = new BulletMeshEntity(bulletWorld, Resource::loadMeshFromObj(s).at(0));
		obj->setColliderAsMesh(Resource::loadMeshFromObj(s).at(0), false);
		obj->createRigidBody(0);
		obj->setShader(shader, true);
		childTransform->addChild(obj);
	}
	
	ragdoll->translatePhysical(glm::vec3(0, 0.5,0));

	PD_Button * button = new PD_Button(bulletWorld, this);
	childTransform->addChild(button);
	button->onDownFunction = [](NodeUI * _this) {
		std::cout << "test " << std::endl;
		std::cout << _this << std::endl;
	};
	
	BulletMeshEntity * obj = new BulletMeshEntity(bulletWorld, Resource::loadMeshFromObj("../assets/S-Tengine2_logo.obj").at(0));
	obj->setColliderAsCapsule();
	obj->createRigidBody(4);
	obj->setShader(shader, true);
	childTransform->addChild(obj);

	Character * c = new Character(bulletWorld);
	c->setShader(shader, true);
	childTransform->addChild(c);
	c->attachJoints();
	c->body->setAngularFactor(btVector3(0,1,0));
	//PD_Story("../assets/the legend of the figure skater's book.json");

	label = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 100);
	label->setMarginTop(2.0f);
	label->setMarginLeft(5.0f);

	Label * label2 = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 100);
	label2->setMarginTop(2.0f);
	label2->setMarginLeft(5.0f);

	Label * label3 = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 100);
	label3->setMarginTop(2.0f);
	label3->setMarginLeft(5.0f);

	Label * label4 = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 150);
	label4->setMarginTop(2.0f);
	label4->setMarginLeft(5.0f);

	Label * label5 = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 100);
	label5->setMarginTop(2.0f);
	label5->setMarginLeft(0.0f);

	Label * label6 = new Label(bulletWorld, this, font, textShader, backgroundShader, WrapMode::CHARACTER_WRAP, 150);
	label6->setMarginTop(2.0f);
	label6->setMarginLeft(5.0f);

	LinearLayout * l = new LinearLayout(VERTICAL, bulletWorld, this);
	l->addChild(label2);
	// Label causes a problem for some reason
	//l->addChild(label);

	LinearLayout * l2 = new LinearLayout(VERTICAL, bulletWorld, this);
	l2->addChild(label3);
	l2->addChild(label4);

	LinearLayout * l3 = new LinearLayout(HORIZONTAL, bulletWorld, this);
	l3->addChild(label5);
	l3->addChild(l);
	l3->addChild(label6);
	l3->addChild(l2);

	childTransform->addChild(l3);

	l3->setMarginRight(0.f);
	
	label2->setText(L"label 2");
	label3->setText(L"label 3");
	label4->setText(L"label 4");
	label5->setText(L"label 5");
	label6->setText(L"label 6");
}


BulletMeshEntity * PD_TestScene::addThing(){
	//static MeshInterface * mesh = Resource::loadMeshFromObj("../assets/pipe.obj").at(0);
	static MeshInterface * mesh = MeshFactory::getCubeMesh(1);
	BulletMeshEntity * thing = new BulletMeshEntity(bulletWorld, mesh);
	
	//thing->setColliderAsMesh(mesh, true);
	thing->setColliderAsBoundingBox();
	//thing->setColliderAsSphere(3);sou
	thing->createRigidBody(1);
	childTransform->addChild(thing);
	thing->setShader(shader, true);
	thing->body->translate(btVector3(std::rand() % 10, 50, std::rand() % 10));
	return thing;
}

PD_TestScene::~PD_TestScene(){
	deleteChildTransform();
	shader->safeDelete();
	//delete phongMat;

	screenSurface->safeDelete();
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
	delete joy;

	delete bulletWorld;

	delete font;
}


void PD_TestScene::update(Step * _step){
	PD_ResourceManager::scene->update(_step);
	/*if(ragdoll->body->body->getWorldTransform().getOrigin().y() < 25){
		ragdoll->body->body->applyImpulse(btVector3(0,5,0), ragdoll->body->body->getWorldTransform().getOrigin());
	}*/

	glm::vec3 pos(0);
	pos += ragdoll->head->getWorldPos();
	pos += ragdoll->upperbody->getWorldPos();
	pos += ragdoll->lowerbody->getWorldPos();
	pos += ragdoll->lowerarmLeft->getWorldPos();
	pos += ragdoll->lowerarmRight->getWorldPos();
	pos += ragdoll->lowerlegLeft->getWorldPos();
	pos += ragdoll->lowerlegRight->getWorldPos();
	pos /= 7;

	pos -= mouseCam->forwardVectorRotated * 2.f;
	pos = (pos - mouseCam->parents.at(0)->getTranslationVector()) * 0.5f;
	mouseCam->parents.at(0)->translate(pos);


	if(mouse->leftDown()){
		float range = 1000;
		glm::vec3 pos = activeCamera->getWorldPos();
		btVector3 start(pos.x, pos.y, pos.z);
		btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
		btVector3 end = start + dir*range;
		btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
		bulletWorld->world->rayTest(start, end, RayCallback);
		if(RayCallback.hasHit()){
			NodeBulletBody * me = static_cast<NodeBulletBody *>(RayCallback.m_collisionObject->getUserPointer());
			if(me != nullptr){
				me->body->activate(true);
				me->body->applyImpulse(dir*-10, me->body->getWorldTransform().getOrigin());
			}
			//std::cout << RayCallback.m_collisionObject->getWorldTransform().getOrigin().x() << std::endl;
		}
	}

	// handle inputs

	joy->update(_step);

	if(keyboard->keyJustDown(GLFW_KEY_BACKSPACE)){
		if(label->getText().size() > 0){
			label->setText(label->getText().substr(0, label->getText().size() - 1));
		}
	}
	if(keyboard->keyJustUp(GLFW_KEY_ENTER)) {
		std::wstring s;
		s += '\n';
		label->appendText(s);
	}
	if(keyboard->justReleasedKeys.size() > 0){
		std::wstringstream acc;
		for(auto k : keyboard->justReleasedKeys){
			if(k.second < 256 && CharacterUtils::isSymbolLetterDigit(k.second) || CharacterUtils::isSpace(k.second)){
				if(!keyboard->shift){
					acc << (wchar_t)tolower(k.second);
				}else {
					acc << (wchar_t)k.second;
				}
			}
		}
		if(acc.tellp() > 0){
			label->appendText(acc.str());
		}
	}
	
	/*if(keyboard->keyJustUp(GLFW_KEY_P)){
		OpenAL_Sound * stream = new OpenAL_Sound("../assets/HighCountdown_Zero2.ogg");
		childTransform->addChild(stream, false);
	}*/
	if(keyboard->keyJustUp(GLFW_KEY_V)){
		PD_ResourceManager::scene->play(true);
	}
	if(keyboard->keyJustUp(GLFW_KEY_B)){
		PD_ResourceManager::scene->pause();
	}
	if(keyboard->keyJustUp(GLFW_KEY_N)){
		PD_ResourceManager::scene->stop();
	}
	if(keyboard->keyJustUp(GLFW_KEY_M)){
		PD_ResourceManager::scene->play();
	}

	if(keyboard->keyJustUp(GLFW_KEY_U)){
		PD_ResourceManager::stream->play(true);
	}
	if(keyboard->keyJustUp(GLFW_KEY_I)){
		PD_ResourceManager::stream->pause();
	}
	if(keyboard->keyJustUp(GLFW_KEY_O)){
		PD_ResourceManager::stream->stop();
	}
	if(keyboard->keyJustUp(GLFW_KEY_P)){
		PD_ResourceManager::stream->play();
	}
	
	glm::vec3 curpos = activeCamera->getWorldPos();
	NodeOpenAL::setListenerVelocity((curpos - lastPos));
	lastPos = curpos;

	NodeOpenAL::setListenerPosition(activeCamera->getWorldPos());
	NodeOpenAL::setListenerOrientation(activeCamera->forwardVectorRotated, activeCamera->upVectorRotated);

	
	if(keyboard->keyJustUp(GLFW_KEY_E)){	
		std::wcout << L"Calling RequestJSONValueAsync..." << std::endl;
		RequestJSONValueAsync(label);
	}
	if(keyboard->keyJustUp(GLFW_KEY_R)){	
		std::stringstream sql;
		sql << "DROP TABLE TestTable;";
		sql << "CREATE TABLE TestTable(id INTEGER PRIMARY KEY, TestColumn1, TestColumn2);";
		for(unsigned long int i = 0; i < 1000; ++i){
			sql << "INSERT INTO TestTable VALUES(" << i << ", 'test1', 'test2');";
		}
		sql << "SELECT * FROM TestTable;";
		testSql(sql.str(), true);
	}

	if(keyboard->keyJustUp(GLFW_KEY_F)){
		firstPerson = !firstPerson;
	}
	
	if(keyboard->keyJustUp(GLFW_KEY_G)){
		addThing();
	}
	if(keyboard->keyJustUp(GLFW_KEY_R)){
		static_cast<ShaderComponentText *>(textShader->getComponentAt(0))->setColor(glm::vec3(1, 0.1, 0.2));
	}
	if(keyboard->keyJustUp(GLFW_KEY_B)){
		static_cast<ShaderComponentText *>(textShader->getComponentAt(0))->setColor(glm::vec3(0.2, 0.1, 1));
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

	if(firstPerson){
		float playerSpeed = 10.f;
		float mass = ragdoll->upperbody->body->getInvMass();

		//mouseCam->parents.at(0)->translate(player->getWorldPos() + glm::vec3(0, 0, player->parents.at(0)->getScaleVector().z*1.25f), false);
		//mouseCam->lookAtOffset = glm::vec3(0, 0, -player->parents.at(0)->getScaleVector().z*0.25f);
		
		glm::vec3 movement(0);
		if (keyboard->keyDown(GLFW_KEY_W)){
			movement += playerSpeed * mass * mouseCam->forwardVectorRotated;
		}
		if (keyboard->keyDown(GLFW_KEY_S)){
			movement -= playerSpeed * mass * mouseCam->forwardVectorRotated;
		}
		if (keyboard->keyDown(GLFW_KEY_A)){
			movement -= playerSpeed * mass * mouseCam->rightVectorRotated;
		}
		if (keyboard->keyDown(GLFW_KEY_D)){
			movement += playerSpeed * mass * mouseCam->rightVectorRotated;
		}

		Joystick * one = joy->joysticks[0];
		if(one != nullptr){
			movement += playerSpeed * mass * mouseCam->forwardVectorRotated * -one->getAxis(Joystick::xbox_axes::kLY);
			movement += playerSpeed * mass * mouseCam->rightVectorRotated * one->getAxis(Joystick::xbox_axes::kLX);
			
			// move camera by directly moving mouse
			float x2 = one->getAxis(Joystick::xbox_axes::kRX)*100;
			float y2 = one->getAxis(Joystick::xbox_axes::kRY)*100;
			mouse->translate(glm::vec2(x2, y2));
		}

		if(movement.x != 0 || movement.y != 0 || movement.z != 0){
			ragdoll->upperbody->body->activate(true);
			ragdoll->upperbody->body->applyCentralImpulse(btVector3(movement.x, movement.y, movement.z));
		}
	}
	

	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_1)){
		if(activeCamera == gameCam){
			activeCamera = mouseCam;
		}else if(activeCamera == mouseCam){
			activeCamera = debugCam;
		}else{
			activeCamera = gameCam;
		}
	}
	if(keyboard->keyJustUp(GLFW_KEY_2)){
		Transform::drawTransforms = !Transform::drawTransforms;
		if(debugDrawer != nullptr){
			bulletWorld->world->setDebugDrawer(nullptr);
			childTransform->removeChild(debugDrawer->parents.at(0));
			delete debugDrawer->parents.at(0);
			debugDrawer = nullptr;
		}else{
			debugDrawer = new BulletDebugDrawer(bulletWorld->world);
			childTransform->addChild(debugDrawer);
			debugDrawer->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
			bulletWorld->world->setDebugDrawer(debugDrawer);
		}
	}
	
	// update scene and physics
	bulletWorld->update(_step);
	Scene::update(_step);

	// update ui stuff
	glm::uvec2 sd = vox::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	uiLayer.update(_step);

	glm::vec3 sp = activeCamera->worldToScreen(ragdoll->head->getWorldPos(), sd);
	if(sp.z < 0){
		sp.z = activeCamera->farClip * 2;
	}
	playerIndicator->parents.at(0)->translate(sp, false);
	crosshair->parents.at(0)->translate(sd.x*0.5f, sd.y*0.5f, 0, false);
	mouseIndicator->parents.at(0)->translate(sd.x - mouse->mouseX(), sd.y - mouse->mouseY(), 0, false);
	
	float volume = std::max(
		std::abs(PD_ResourceManager::scene->getAmplitude()),
		std::abs(PD_ResourceManager::stream->getAmplitude())
	);
	volumeIndicator->parents.at(0)->translate(sd.x, sd.y, 0, false);
	volumeIndicator->parents.at(0)->scale(50, volume*sd.y*0.5f, 1, false);
}

void PD_TestScene::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	clear();
	
	//float scale = 1;
	//game->setViewport(0, 0, game->viewPortWidth * 1 / scale, game->viewPortHeight * 1 / scale);
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);

	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	Scene::render(_matrixStack, _renderOptions);
	//game->setViewport(0, 0, game->viewPortWidth*scale, game->viewPortHeight*scale);

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