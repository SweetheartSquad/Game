#pragma once

#include <ftglyph.h>

#include <PD_TestScene.h>
#include <PD_Game.h>
#include <PD_ResourceManager.h>
#include <PD_Player.h>
#include <PD_ContactListener.h>

#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <Material.h>

#include <shader\BaseComponentShader.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentPhong.h>
#include <shader\ShaderComponentBlinn.h>
#include <shader\ShaderComponentShadow.h>
#include <shader\ShaderComponentHsv.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDraw.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <Sound.h>
#include <libzplay.h>

#include <Keyboard.h>
#include <GLFW\glfw3.h>
#include <MatrixStack.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>

FT_Face face;

PD_TestScene::PD_TestScene(Game * _game) :
	Scene(_game),
	shader(new BaseComponentShader(true)),
	world(new Box2DWorld(b2Vec2(0, 0))),
	drawer(nullptr),
	player(nullptr),
	sceneHeight(150),
	sceneWidth(50),
	firstPerson(true),
	screenSurfaceShader(new Shader("../assets/RenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	phongMat(new Material(15.0, glm::vec3(1.f, 1.f, 1.f), true)),
	hsvComponent(new ShaderComponentHsv(shader, 0, 1, 1))
{
	shader->components.push_back(new ShaderComponentTexture(shader));
	shader->components.push_back(new ShaderComponentDiffuse(shader));
	shader->components.push_back(hsvComponent);
	//shader->components.push_back(new ShaderComponentPhong(shader));
	//shader->components.push_back(new ShaderComponentBlinn(shader));
	//shader->components.push_back(new ShaderComponentShadow(shader));
	shader->compileShader();

	clearColor[0] = 1.f;
	clearColor[1] = 1.f;
	clearColor[2] = 1.f;
	clearColor[3] = 1.f;

	//Set up cameras
	mouseCam = new MousePerspectiveCamera();
	cameras.push_back(mouseCam);
	mouseCam->farClip = 1000.f;
	mouseCam->nearClip = 0.001f;
	mouseCam->transform->rotate(90, 0, 1, 0, kWORLD);
	mouseCam->transform->translate(5.0f, 1.5f, 22.5f);
	mouseCam->yaw = 90.0f;
	mouseCam->pitch = -10.0f;
	mouseCam->speed = 1;

	debugCam = new MousePerspectiveCamera();
	cameras.push_back(debugCam);
	debugCam->farClip = 1000.f;
	debugCam->transform->rotate(90, 0, 1, 0, kWORLD);
	debugCam->transform->translate(5.0f, 1.5f, 22.5f);
	debugCam->yaw = 90.0f;
	debugCam->pitch = -10.0f;
	debugCam->speed = 1;

	gameCam = new FollowCamera(15, glm::vec3(0, 0, 0), 0, 0);
	cameras.push_back(gameCam);
	gameCam->farClip = 1000.f;
	gameCam->transform->rotate(90, 0, 1, 0, kWORLD);
	gameCam->transform->translate(5.0f, 1.5f, 22.5f);
	gameCam->minimumZoom = 22.5f;
	gameCam->yaw = 90.0f;
	gameCam->pitch = -10.0f;

	activeCamera = mouseCam;
	
	float _size = 3;
	std::vector<Box2DMeshEntity *> boundaries;
	MeshInterface * boundaryMesh = MeshFactory::getPlaneMesh();
	boundaries.push_back(new Box2DMeshEntity(world, MeshFactory::getPlaneMesh(), b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(world, MeshFactory::getPlaneMesh(), b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(world, MeshFactory::getPlaneMesh(), b2_staticBody));
	boundaries.push_back(new Box2DMeshEntity(world, MeshFactory::getPlaneMesh(), b2_staticBody));

	boundaries.at(0)->transform->scale(_size, sceneHeight*0.5f + _size*2.f, _size * 4.f);
	boundaries.at(1)->transform->scale(_size, sceneHeight*0.5f + _size*2.f, _size * 4.f);
	boundaries.at(2)->transform->scale(sceneWidth*0.5f + _size*2.f, _size, _size * 4.f);
	boundaries.at(3)->transform->scale(sceneWidth*0.5f + _size*2.f, _size, _size * 4.f);

	boundaries.at(0)->setTranslationPhysical(sceneWidth+_size, sceneHeight*0.5f, 0);
	boundaries.at(1)->setTranslationPhysical(-_size, sceneHeight*0.5f, 0);
	boundaries.at(2)->setTranslationPhysical(sceneWidth*0.5f, sceneHeight+_size, 0);
	boundaries.at(3)->setTranslationPhysical(sceneWidth*0.5f, -_size, 0);
	
	b2Filter sf;
	//sf.categoryBits = PuppetGame::kBOUNDARY;
	//sf.maskBits = -1;
	for(auto b : boundaries){
		addChild(b);
		b->setShader(shader, true);
		world->addToWorld(b);
		b->body->GetFixtureList()->SetFilterData(sf);
		b->mesh->pushMaterial(phongMat);
		//b->mesh->pushTexture2D(PuppetResourceManager::stageFront);
	}
	//sf.categoryBits = PuppetGame::kBOUNDARY | PuppetGame::kGROUND;
	boundaries.at(3)->body->GetFixtureList()->SetFilterData(sf);
	boundaries.at(3)->body->GetFixtureList()->SetFriction(1);
	boundaries.at(3)->body->GetFixtureList()->SetRestitution(0);

	ground = new MeshEntity(MeshFactory::getPlaneMesh());
	ground->transform->translate(sceneWidth/2.f, sceneHeight/2.f, -2.f);
	ground->transform->scale(sceneWidth, sceneWidth, 1);
	ground->setShader(shader, true);
	addChild(ground);

	MeshEntity * ceiling = new MeshEntity(MeshFactory::getPlaneMesh());
	ceiling->transform->translate(sceneWidth/2.f, sceneHeight/2.f, _size * 4.f);
	ceiling->transform->scale(sceneWidth, sceneHeight, 1);
	ceiling->setShader(shader, true);
	//addChild(ceiling);


	//lights.push_back(new DirectionalLight(glm::vec3(1,0,0), glm::vec3(1,1,1), 0));
	
	player = new PD_Player(world);
	player->setShader(shader, true);
	gameCam->addTarget(player, 1);
	addChild(player);
	player->setTranslationPhysical(sceneWidth / 2.f, sceneHeight / 8.f, 0, false);
	
	//intialize key light
	PointLight * keyLight = new PointLight(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(1.f, 1.f, 1.f), 0.00f, 0.01f, -10.f);
	//Set it as the key light so it casts shadows
	//keyLight->isKeyLight = true;
	//Add it to the scene
	lights.push_back(keyLight);
	player->addChild(keyLight);
	
	mouseCam->upVectorLocal = glm::vec3(0, 0, 1);
	mouseCam->forwardVectorLocal = glm::vec3(1, 0, 0);
	mouseCam->rightVectorLocal = glm::vec3(0, -1, 0);

	PD_ContactListener * cl = new PD_ContactListener(this);
	world->b2world->SetContactListener(cl);

	addChild(player);

	
	ft_lib = nullptr;
	if(FT_Init_FreeType(&ft_lib) != 0) {
		std::cerr << "Couldn't initialize FreeType library\n";
	}

	face = nullptr;
	if(FT_New_Face(ft_lib, "../assets/arial.ttf", 0, &face) != 0) {
		std::cerr << "Couldn't initialize FreeType library\n";
	}
}

void PD_TestScene::render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy) {

	 FT_Set_Pixel_Sizes(face, 0, 200);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //static FT_GlyphSlot glyph = face->glyph;

	//FT_Load_Glyph(face, 'R', FT_LOAD_DEFAULT);

	//FT_Glyph glyph;

	const FT_GlyphSlot glyph = face->glyph;

	FT_Load_Char(face, 'p', FT_LOAD_RENDER);
	//if(FT_Get_Glyph(face->glyph, &glyph ))
		//throw std::runtime_error("FT_Get_Glyph failed");

//int ii = FT_Load_Char(face, 'R', FT_LOAD_RENDER);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	//FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
	
	//FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	//if(ii != 0){
		Texture * tex = new Texture(glyph->bitmap, true, false);
		tex->load();
		while(ground->mesh->textureCount() > 0){
			ground->mesh->popTexture2D();
		}
		ground->mesh->pushTexture2D(tex);
	//}

	float xr=(float)glyph->bitmap.width;
    float yr=(float)glyph->bitmap.rows;
		
	float vx = x + glyph->bitmap_left * sx;
	float vy = y + glyph->bitmap_top * sy;
    float w = glyph->bitmap.width * sx;
    float h = glyph->bitmap.rows * sy;
	
	
	//ground->mesh->polygonalDrawMode = GL_TRIANGLES;

	ground->mesh->vertices.clear();
	ground->mesh->indices.clear();

	ground->mesh->pushVert(Vertex(glm::vec3(vx, vy, -2.f), glm::vec2(0.f, 0.f)));
	ground->mesh->pushVert(Vertex(glm::vec3(vx + w, vy, -2.f), glm::vec2(0.5f, 0.f)));
	ground->mesh->pushVert(Vertex(glm::vec3(vx + w, vy - h, -2.f), glm::vec2(0.5f, 0.5f)));
	ground->mesh->pushVert(Vertex(glm::vec3(vx, vy-h, -2.f), glm::vec2(0.f, 0.5f)));


	ground->mesh->dirty = true;

    /*for(auto c : str) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = x + glyph->bitmap_left * sx;
        const float vy = y + glyph->bitmap_top * sy;
        const float w = glyph->bitmap.width * sx;
        const float h = glyph->bitmap.rows * sy;

        struct {
            float x, y, s, t;
        } data[6] = {
            {vx    , vy    , 0, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy    , 1, 0},
            {vx + w, vy    , 1, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy - h, 1, 1}
        };

        //glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), data, GL_DYNAMIC_DRAW);
        //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (glyph->advance.x >> 6) * sx;
        y += (glyph->advance.y >> 6) * sy;
    }*/

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
}


PD_TestScene::~PD_TestScene(){
	while(children.size() > 0){
		//NodeHierarchical::deleteRecursively(children.back());
		//children.pop_back();
	}
	
	shader->safeDelete();
	//delete phongMat;
	delete world;

	delete screenSurface;
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
}

void PD_TestScene::update(Step * _step){

	FT_Set_Pixel_Sizes(face, 0, 50);
    render_text("Hello World!", face, -0.5, 0, 2.f/640.f, 2.f/480.f);
	
	if(keyboard->keyJustUp(GLFW_KEY_F11)){
		game->toggleFullScreen();
	}

	if(keyboard->keyJustUp(GLFW_KEY_F)){
		firstPerson = !firstPerson;
	}

	// camera controls
	if (keyboard->keyDown(GLFW_KEY_UP)){
		activeCamera->transform->translate((activeCamera->forwardVectorRotated) * static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_DOWN)){
		activeCamera->transform->translate((activeCamera->forwardVectorRotated) * -static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_LEFT)){
		activeCamera->transform->translate((activeCamera->rightVectorRotated) * -static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}
	if (keyboard->keyDown(GLFW_KEY_RIGHT)){
		activeCamera->transform->translate((activeCamera->rightVectorRotated) * static_cast<MousePerspectiveCamera *>(activeCamera)->speed);
	}

	if(firstPerson){
		float playerSpeed = 2.5f;
		float mass = player->body->GetMass();
		float angle = atan2(mouseCam->forwardVectorRotated.y, mouseCam->forwardVectorRotated.x);

		if(activeCamera != mouseCam){
			angle = glm::radians(90.f);
		}

		mouseCam->transform->translate(player->getPos(false) + glm::vec3(0, 0, player->transform->getScaleVector().z*1.25f), false);
		mouseCam->lookAtOffset = glm::vec3(0, 0, -player->transform->getScaleVector().z*0.25f);
		
		
		if (keyboard->keyDown(GLFW_KEY_W)){
			player->applyLinearImpulseUp(playerSpeed * mass * sin(angle));
			player->applyLinearImpulseRight(playerSpeed * mass * cos(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_S)){
			player->applyLinearImpulseDown(playerSpeed * mass * sin(angle));
			player->applyLinearImpulseLeft(playerSpeed * mass * cos(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_A)){
			player->applyLinearImpulseUp(playerSpeed * mass * cos(angle));
			player->applyLinearImpulseLeft(playerSpeed * mass * sin(angle));
		}
		if (keyboard->keyDown(GLFW_KEY_D)){
			player->applyLinearImpulseDown(playerSpeed * mass * cos(angle));
			player->applyLinearImpulseRight(playerSpeed * mass * sin(angle));
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
		if(drawer != nullptr){
			world->b2world->SetDebugDraw(nullptr);
			removeChild(drawer);
			delete drawer;
			drawer = nullptr;
		}else{
			drawer = new Box2DDebugDraw(world);
			world->b2world->SetDebugDraw(drawer);
			drawer->drawing = true;
			//drawer->AppendFlags(b2Draw::e_aabbBit);
			drawer->AppendFlags(b2Draw::e_shapeBit);
			drawer->AppendFlags(b2Draw::e_centerOfMassBit);
			drawer->AppendFlags(b2Draw::e_jointBit);
			//drawer->AppendFlags(b2Draw::e_pairBit);
			addChild(drawer);
		}
	}
	
	Scene::update(_step);
	world->update(_step);
}

void PD_TestScene::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	Scene::render(_matrixStack, _renderOptions);

	//Render the buffer to the render surface
	screenSurface->render(screenFBO->getTextureId());
}

void PD_TestScene::load(){
	Scene::load();	

	screenSurface->load();
	screenFBO->load();
}

void PD_TestScene::unload(){
	Scene::unload();	

	screenSurface->unload();
	screenFBO->unload();
}