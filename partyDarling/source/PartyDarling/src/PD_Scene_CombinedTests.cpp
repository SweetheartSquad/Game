#pragma once

#include <PD_Scene_CombinedTests.h>
#include <PD_ResourceManager.h>
#include <PD_Game.h>
#include <PD_Assets.h>
#include <Resource.h>
#include <shader/ShaderComponentMVP.h>
#include <shader/ShaderComponentTexture.h>
#include <shader/ShaderComponentDiffuse.h>
#include <shader/ShaderComponentIndexedTexture.h>
#include <shader/ShaderComponentDepthOffset.h>
#include <shader/ComponentShaderText.h>

#include <NumberUtils.h>
#include <StringUtils.h>
#include <TextureUtils.h>
#include <sweet/UI.h>

#include <MousePerspectiveCamera.h>
#include <MeshFactory.h>
#include <Timeout.h>
#include <MatrixStack.h>

#include <RenderOptions.h>
#include <json\json.h>

#include <sweet/Input.h>

#include <PointLight.h>


struct OculusEye{
	ovrSizei			size;
	GLuint              fboId;
	struct TextureBuffer{
		ovrSwapTextureSet*  TextureSet;
		GLuint              texId;
	} tbuffer;
	struct DepthBuffer{
		GLuint        texId;
	} dbuffer;
} eyes[2];
ovrVector3f ViewOffset[2];
ovrPosef    EyeRenderPose[2];
double ftiming;
double sensorSampleTime;
float eyeSeparationScale = 0;
float eyeSeparationScale_ui = 1.8;

PD_Scene_CombinedTests::PD_Scene_CombinedTests(Game * _game) :
	Scene(_game),
	uiLayer(0,0,0,0),
	shader(new ComponentShaderBase(false)),
	characterShader(new ComponentShaderBase(false)),
	bulletWorld(new BulletWorld()),
	debugDrawer(nullptr),
	currentHoverTarget(nullptr),
	selectedItem(nullptr)
{
	shader->addComponent(new ShaderComponentMVP(shader));
	shader->addComponent(new ShaderComponentTexture(shader, 0));
	shader->addComponent(new ShaderComponentDiffuse(shader));
	shader->compileShader();

	characterShader->addComponent(new ShaderComponentMVP(characterShader));
	characterShader->addComponent(new ShaderComponentIndexedTexture(characterShader));
	characterShader->addComponent(new ShaderComponentDepthOffset(characterShader));
	characterShader->compileShader();

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);


	// remove initial camera
	childTransform->removeChild(cameras.at(0)->parents.at(0));
	delete cameras.at(0)->parents.at(0);
	cameras.pop_back();

	// add crosshair
	VerticalLinearLayout * l = new VerticalLinearLayout(uiLayer.world);
	l->setRationalHeight(1.f);
	l->setRationalWidth(1.f);
	l->horizontalAlignment = kCENTER;
	l->verticalAlignment = kMIDDLE;

	crosshairIndicator = new NodeUI(uiLayer.world);
	crosshairIndicator->background->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);
	crosshairIndicator->setWidth(16);
	crosshairIndicator->setHeight(16);
	crosshairIndicator->background->mesh->setScaleMode(GL_NEAREST);
	uiLayer.addChild(l);
	l->addChild(crosshairIndicator);

	for(unsigned long int i = 0; i < 50; ++i){
		std::map<std::string, Asset *>::iterator itemDef = PD_ResourceManager::scenario->assets["item"].begin();
		if(itemDef != PD_ResourceManager::scenario->assets["item"].end()){
			std::advance(itemDef, sweet::NumberUtils::randomInt(0, PD_ResourceManager::scenario->assets["item"].size()-1));
			PD_Item * item = dynamic_cast<AssetItem *>(itemDef->second)->getItem(bulletWorld, shader);
			item->addToWorld();
			childTransform->addChild(item);
	
			item->setTranslationPhysical(sweet::NumberUtils::randomFloat(-50, 50), 2, sweet::NumberUtils::randomFloat(-50, 50));
			item->rotatePhysical(45,0,1,0,false);
		}
	}
	

	uiBubble = new PD_UI_Bubble(uiLayer.world);
	uiLayer.addChild(uiBubble);

	uiBubble->addOption("test", nullptr);

	uiInventory = new PD_UI_Inventory(uiLayer.world);
	uiLayer.addChild(uiInventory);
	uiInventory->eventManager.addEventListener("itemSelected", [this](sweet::Event * _event){
		uiInventory->close();
		uiLayer.removeMouseIndicator();

		// replace the crosshair texture with the item texture
		crosshairIndicator->background->mesh->replaceTextures(uiInventory->getSelected()->mesh->textures.at(0));
		// TODO: update the UI to indicate the selected item to the player
	});

	uiDialogue = new PD_UI_Dialogue(uiLayer.world, uiBubble);
	uiLayer.addChild(uiDialogue);
	uiDialogue->setRationalHeight(1.f, &uiLayer);
	uiDialogue->setRationalWidth(1.f, &uiLayer);
	uiDialogue->eventManager.addEventListener("end", [this](sweet::Event * _event){
		player->enable();
	});

	uiYellingContest = new PD_UI_YellingContest(uiLayer.world, PD_ResourceManager::scenario->defaultFont->font, uiBubble->textShader, uiLayer.shader);
	uiLayer.addChild(uiYellingContest);
	uiYellingContest->setRationalHeight(1.f, &uiLayer);
	uiYellingContest->setRationalWidth(1.f, &uiLayer);
	uiYellingContest->eventManager.addEventListener("complete", [this](sweet::Event * _event){
		uiYellingContest->disable();
		player->enable();
	});

	PersonRenderer * testCharacter = new PersonRenderer(bulletWorld);
	childTransform->addChild(testCharacter);
	testCharacter->setShader(characterShader, true);
	testCharacter->unload();
	testCharacter->load();
	testCharacter->firstParent()->scale(0.001f);
	testCharacter->firstParent()->translate(0, 2, 0);
	testCharacter->butt->setTranslationPhysical(0, 2, 0, true);





	// add a ground to the scene
	BulletMeshEntity * bulletGround = new BulletMeshEntity(bulletWorld, MeshFactory::getPlaneMesh(), shader);
	bulletGround->setColliderAsStaticPlane(0, 1, 0, 0);
	bulletGround->createRigidBody(0);
	childTransform->addChild(bulletGround);
	bulletGround->meshTransform->scale(1000,1000,1000);
	bulletGround->meshTransform->rotate(-90, 1, 0, 0, kOBJECT);
	bulletGround->body->translate(btVector3(0, 0, 0));
	bulletGround->body->setFriction(1);
	bulletGround->mesh->pushTexture2D(PD_ResourceManager::scenario->getTexture("GREY")->texture);

	// add the player to the scene
	player = new Player(bulletWorld);
	childTransform->addChild(player);
	cameras.push_back(player->playerCamera);
	activeCamera = player->playerCamera;
	childTransform->addChild(player->playerCamera);
	player->playerCamera->firstParent()->translate(0, 5, 0);


	PointLight * light = new PointLight(glm::vec3(1.f), 0.f, 0.01f, -1.f);
	lights.push_back(light);
	childTransform->addChild(light)->translate(0, 5, 0);


	// oculus setup
	if(sweet::ovrInitialized){
		for(unsigned long int eye = 0; eye < 2; ++eye){
			//eyes[eye].size.w = sweet::hmdDesc.Resolution.w/2;
			//eyes[eye].size.h = sweet::hmdDesc.Resolution.h/2;
			eyes[eye].size = ovr_GetFovTextureSize(*sweet::hmd, ovrEyeType(eye), sweet::hmdDesc.DefaultEyeFov[eye], 1);
			checkForOvrError(ovr_CreateSwapTextureSetGL(*sweet::hmd, GL_SRGB8_ALPHA8, eyes[eye].size.w, eyes[eye].size.h, &eyes[eye].tbuffer.TextureSet));

			for (int i = 0; i < eyes[eye].tbuffer.TextureSet->TextureCount; ++i){
				ovrGLTexture* tex = (ovrGLTexture*)&eyes[eye].tbuffer.TextureSet->Textures[i];
				glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);
		
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glGenFramebuffers(1, &eyes[eye].fboId);


			// depth
			glGenTextures(1, &eyes[eye].dbuffer.texId);
			glBindTexture(GL_TEXTURE_2D, eyes[eye].dbuffer.texId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLenum internalFormat = GL_DEPTH_COMPONENT24;
			GLenum type = GL_UNSIGNED_INT;
			/*if (GLE_ARB_depth_buffer_float)
			{
				internalFormat = GL_DEPTH_COMPONENT32F;
				type = GL_FLOAT;
			}*/

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, eyes[eye].size.w, eyes[eye].size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
   
		}
	}

	for(unsigned long int i = 0; i < 100; ++i){
		Transform * t = childTransform->addChild(new MeshEntity(MeshFactory::getCubeMesh(), shader));
		t->scale(sweet::NumberUtils::randomFloat(1, 10));
		t->translate(sweet::NumberUtils::randomFloat(-100, 100), sweet::NumberUtils::randomFloat(-100, 100), sweet::NumberUtils::randomFloat(-100, 100));
	}
}

PD_Scene_CombinedTests::~PD_Scene_CombinedTests(){
	deleteChildTransform();

	if(sweet::ovrInitialized){
		for(unsigned long int eye = 0; eye < 2; ++eye){
			ovr_DestroySwapTextureSet(*sweet::hmd, eyes[eye].tbuffer.TextureSet);
			eyes[eye].tbuffer.TextureSet = nullptr;
			glDeleteFramebuffers(1, &eyes[eye].fboId);
			eyes[eye].fboId = 0;
			glDeleteTextures(1, &eyes[eye].dbuffer.texId);
			eyes[eye].dbuffer.texId = 0;
		}
	}
}

void PD_Scene_CombinedTests::update(Step * _step){
	bulletWorld->update(_step);

	
	if(sweet::ovrInitialized){
		// Get eye poses, feeding in correct IPD offset
		ViewOffset[0] = sweet::EyeRenderDesc[0].HmdToEyeViewOffset;
		ViewOffset[1] = sweet::EyeRenderDesc[1].HmdToEyeViewOffset;
		ftiming = ovr_GetPredictedDisplayTime(*sweet::hmd, 0);
		ovrTrackingState hmdState = ovr_GetTrackingState(*sweet::hmd, ftiming, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);
		sensorSampleTime = ovr_GetTimeInSeconds();

		//player->playerCamera->firstParent()->translate(EyeRenderPose[0].Position.x, EyeRenderPose[0].Position.y, EyeRenderPose[0].Position.z);
		//player->playerCamera->childTransform->setOrientation(glm::quat(EyeRenderPose[0].Orientation.w, EyeRenderPose[0].Orientation.z, EyeRenderPose[0].Orientation.y, EyeRenderPose[0].Orientation.x));
	
		if(keyboard->keyJustDown(GLFW_KEY_R)){
			ovr_RecenterPose(*sweet::hmd);
		}

		if(keyboard->keyJustDown(GLFW_KEY_EQUAL)){
			eyeSeparationScale += 0.1;
		}else if(keyboard->keyJustDown(GLFW_KEY_MINUS)){
			eyeSeparationScale -= 0.1;
		}
	}
	
	// mouse interaction with world objects
	if(player->isEnabled()){
		float range = 4;
		glm::vec3 pos = activeCamera->childTransform->getWorldPos();
		btVector3 start(pos.x, pos.y, pos.z);
		btVector3 dir(activeCamera->forwardVectorRotated.x, activeCamera->forwardVectorRotated.y, activeCamera->forwardVectorRotated.z);
		btVector3 end = start + dir*range;
		btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
		bulletWorld->world->rayTest(start, end, RayCallback);
		
		
		NodeBulletBody * lastHoverTarget = currentHoverTarget;
		if(RayCallback.hasHit()){
			NodeBulletBody * me = static_cast<NodeBulletBody *>(RayCallback.m_collisionObject->getUserPointer());
			
			if(me != nullptr){
				PD_Item * item = dynamic_cast<PD_Item *>(me);
				if(item != nullptr){
					if(item->actuallyHovered(glm::vec3(RayCallback.m_hitPointWorld.getX(), RayCallback.m_hitPointWorld.getY(), RayCallback.m_hitPointWorld.getZ()))){
						// hover over item
						if(item != currentHoverTarget){
							// if we aren't already looking at the item,
							// clear out the bubble UI and add the relevant options
							uiBubble->clear();
							if(item->definition->collectable){
								uiBubble->addOption("pickup", [this, item](sweet::Event * _event){
									// remove the item from the scene
									Transform * toDelete = item->firstParent();
									toDelete->firstParent()->removeChild(toDelete);
									toDelete->removeChild(item);
									delete toDelete;
									delete item->shape;
									item->shape = nullptr;
									bulletWorld->world->removeRigidBody(item->body);
									item->body = nullptr;

									// pickup the item
									uiInventory->pickupItem(item);
								});
							}else{
								uiBubble->addOption("interact", [](sweet::Event * _event){
									std::cout << "hey gj you interacted" << std::endl;
								});
							}
						}
					}else{
						// we hovered over an item, but it wasn't pixel-perfect
						me = item = nullptr;
					}
				}else{
					PersonButt * butt = dynamic_cast<PersonButt *>(me);
					if(butt != nullptr){
						// hover over person
						if(butt != currentHoverTarget){
							// if we aren't already looking at the person,
							// clear out the bubble UI and add the relevant options
							uiBubble->clear();
							uiBubble->addOption("talk to ", [this, butt](sweet::Event * _event){
								uiDialogue->startEvent(butt->person->state.conversation);
								player->disable();
							});
							uiBubble->addOption("yell at ", [this](sweet::Event * _event){
								uiYellingContest->startNewFight();
								uiBubble->clear();
								player->disable();
								// TODO: pass in the character that's fighting here
							});
							// if we have an item, also add the "use on" option
							if(uiInventory->getSelected() != nullptr){
								uiBubble->addOption("use item on character", [this](sweet::Event * _event){
									uiBubble->clear();
									player->disable();
									// TODO: pass in the character that's fighting here
								});
							}
						}
					}
				}
			}

			/*NodeUI * ui = dynamic_cast<NodeUI *>(me);
			if(ui != nullptr){
				ui->setUpdateState(true);
			}*/
		

			currentHoverTarget = me;
		}else{
			currentHoverTarget = nullptr;
		}
		if((lastHoverTarget != currentHoverTarget) && currentHoverTarget == nullptr || selectedItem != uiInventory->getSelected()){
			uiBubble->clear();
			selectedItem = uiInventory->getSelected();
			if(uiInventory->getSelected() != nullptr){
				uiBubble->addOption("use item on self", [this](sweet::Event * _event){
					//uiBubble->clear();
					//player->disable();
					// TODO: actually trigger item interaction
				});
				uiBubble->addOption("drop item", [this](sweet::Event * _event){
					//uiBubble->clear();

					// dropping an item
					if(PD_Item * item = uiInventory->removeSelected()){
						// put the item back into the scene
						childTransform->addChild(item);
						item->addToWorld();
			
						// figure out where to put the item
						glm::vec3 targetPos = activeCamera->getWorldPos() + activeCamera->forwardVectorRotated * 3.f;
						targetPos.y = 2; // always put stuff on the ground
						item->setTranslationPhysical(targetPos, false);
						// rotate the item to face the camera
						item->rotatePhysical(activeCamera->yaw - 90,0,1,0, false);
					}

					// replace the crosshair itme texture with the actual crosshair texture
					crosshairIndicator->background->mesh->replaceTextures(PD_ResourceManager::scenario->getTexture("CROSSHAIR")->texture);


				});
			}
		}
	}
	
	// inventory toggle
	if(keyboard->keyJustDown(GLFW_KEY_TAB)){
		if(uiInventory->isVisible()){
			uiInventory->close();
			uiLayer.removeMouseIndicator();
		}else{
			uiInventory->open();
			uiLayer.addMouseIndicator();
		}
	}

	// bubble testing controls
	if(keyboard->keyJustDown(GLFW_KEY_V)){
		uiBubble->next();
	}if(keyboard->keyJustDown(GLFW_KEY_B)){
		uiBubble->prev();
	}if(keyboard->keyJustDown(GLFW_KEY_N)){
		uiBubble->addOption("test", nullptr);
	}

	// debug controls
	if(keyboard->keyJustDown(GLFW_KEY_F12)){
		game->toggleFullScreen();
	}

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
	
	

	Scene::update(_step);

	glm::uvec2 sd = sweet::getWindowDimensions();
	uiLayer.resize(0,sd.x,0,sd.y);
	uiLayer.update(_step);
}

void PD_Scene_CombinedTests::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){

	_renderOptions->setClearColour(1,0,1,1);

	if(sweet::ovrInitialized){
		// render oculus eyes
		for(unsigned long int eye = 0; eye < 2; ++eye){
			// Increment to use next texture, just before writing
			eyes[eye].tbuffer.TextureSet->CurrentIndex = (eyes[eye].tbuffer.TextureSet->CurrentIndex + 1) % eyes[eye].tbuffer.TextureSet->TextureCount;

			// Switch to eye render target
			// set render surface
			auto tex = reinterpret_cast<ovrGLTexture*>(&eyes[eye].tbuffer.TextureSet->Textures[eyes[eye].tbuffer.TextureSet->CurrentIndex]);

			glBindFramebuffer(GL_FRAMEBUFFER, eyes[eye].fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, eyes[eye].dbuffer.texId, 0);
	
			game->setViewport(0, 0, eyes[eye].size.w, eyes[eye].size.h);
			glEnable(GL_FRAMEBUFFER_SRGB);
	
			// render scene
			_matrixStack->pushMatrix();
		
			activeCamera->firstParent()->translate(-sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.x*eyeSeparationScale, -sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.y, -sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.z);
			uiLayer.childTransform->translate(-sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.x*eyeSeparationScale_ui*sweet::getWindowWidth(), -sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.y, -sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.z);
			
			_renderOptions->clear();
			Scene::render(_matrixStack, _renderOptions);
			uiLayer.render(_matrixStack, _renderOptions);
			_matrixStack->popMatrix();
		
			uiLayer.childTransform->translate(sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.x*eyeSeparationScale_ui*sweet::getWindowWidth(), sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.y, sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.z);
			activeCamera->firstParent()->translate(sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.x*eyeSeparationScale, sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.y, sweet::EyeRenderDesc[eye].HmdToEyeViewOffset.z);
		
			// unset render surface
			glBindFramebuffer(GL_FRAMEBUFFER, eyes[eye].fboId);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			//glDisable(GL_FRAMEBUFFER_SRGB);
		}



		 // Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

		ovrLayerEyeFov ld;
		ld.Header.Type  = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
	
		ovrRecti recti;
		for (int eye = 0; eye < 2; ++eye)
		{
			recti.Size = eyes[eye].size;
			recti.Pos.x = 0;
			recti.Pos.y = 0;
			ld.ColorTexture[eye] = eyes[eye].tbuffer.TextureSet;
			ld.Viewport[eye]     = recti;
			ld.Fov[eye]          = sweet::hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye]   = EyeRenderPose[eye];
			ld.SensorSampleTime  = sensorSampleTime;
		}
		//ld.Viewport[1].Pos.x = windowSize.w;
		//ld.Viewport[1].Pos.y = windowSize.h;

		ovrLayerHeader* layers = &ld.Header;
		checkForOvrError(ovr_SubmitFrame(*sweet::hmd, 0, &viewScaleDesc, &layers, 1));
		//glfwSwapBuffers(sweet::currentContext);



		// Render what the oculus sees to the screen 
		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, sweet::mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = sweet::mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = sweet::mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
							0, 0, w, h,
							GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}else{
		// standard scene rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		_renderOptions->clear();
		Scene::render(_matrixStack, _renderOptions);
		uiLayer.render(_matrixStack, _renderOptions);
	}
	

	glfwSwapBuffers(sweet::currentContext);
}

void PD_Scene_CombinedTests::load(){
	Scene::load();	
	uiLayer.load();
}

void PD_Scene_CombinedTests::unload(){
	uiLayer.unload();
	Scene::unload();	
}