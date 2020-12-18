#include "Scene.h"
#include "Vendor/stb_image.h"
#include <glm/gtx/color_space.hpp>

float terrainXScale = 12000.f;
float terrainYScale = 700.f;
float terrainZScale = 12000.f;

extern bool LMB;
extern bool RMB;
extern bool endLoop;
extern float leftRightMB;
extern float angularFOV;
extern float dt;
extern int winWidth;
extern int winHeight;

void SetUpCubemap(uint& cubemapRefID, const std::vector<cstr>& faces);

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

Scene::Scene():
	cam(glm::vec3(0.0f, 0.f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.f, 1.f, 0.0f), 0.0f, 1400.0f),
	soundEngine(nullptr),
	coinMusic({}),
	coinSoundFX({}),
	fireMusic({}),
	fireSoundFX({}),
	models{
		new Model("ObjsAndMtls/Shotgun.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Scar.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Sniper.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Flower.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Grass.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Rock.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Tree_Low.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Tree_Medium.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Tree_High.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Dragon_Low.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Dragon_Medium.obj", {
			aiTextureType_DIFFUSE,
		}),
		new Model("ObjsAndMtls/Dragon_High.obj", {
			aiTextureType_DIFFUSE,
		}),
	},
	forwardSP{"Shaders/Forward.vertexS", "Shaders/Forward.fragS"},
	textSP{"Shaders/Text.vertexS", "Shaders/Text.fragS"},
	cubemapRefID(0),
	currSlot(0),
	inv{
		ItemType::Shotgun,
		ItemType::Scar,
		ItemType::Sniper,
		ItemType::None,
		ItemType::None,
	},
	screen(Screen::MainMenu),
	textScaleFactors{
		1.f,
		1.f,
	},
	textColours{
		glm::vec4(1.f),
		glm::vec4(1.f),
	},
	currGun(nullptr),
	guns{
		nullptr,
		nullptr,
		nullptr,
	},
	reticleColour(glm::vec4(1.f)),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	isCamDetached(true),
	elapsedTime(0.f),
	modelStack(),
	polyModes(),
	dLightFromTop(nullptr),
	dLightFromBottom(nullptr),
	entityManager(EntityManager::GetObjPtr())
{
}

Scene::~Scene(){
	if(dLightFromTop){
		delete dLightFromTop;
		dLightFromTop = nullptr;
	}
	if(dLightFromBottom){
		delete dLightFromBottom;
		dLightFromBottom = nullptr;
	}

	for(short i = 0; i < 3; ++i){
		if(guns[i]){
			delete guns[i];
			guns[i] = nullptr;
		}
	}
	
	for(int i = 0; i < (int)MeshType::Amt; ++i){
		if(Meshes::meshes[i]){
			delete Meshes::meshes[i];
			Meshes::meshes[i] = nullptr;
		}
	}
	for(int i = 0; i < (int)ModelType::Amt; ++i){
		if(models[i]){
			delete models[i];
			models[i] = nullptr;
		}
	}

	const size_t& coinMusicSize = coinMusic.size();
	for(size_t i = 0; i < coinMusicSize; ++i){
		coinMusic[i]->drop();
	}
	const size_t& fireMusicSize = fireMusic.size();
	for(size_t i = 0; i < fireMusicSize; ++i){
		fireMusic[i]->drop();
	}
	if(soundEngine){
		soundEngine->drop();
	}

	entityManager->Destroy();
}

void Scene::InitEntities(){
	entityManager->Init();

	//* Create Player
	{
		const float scaleFactor = 50.0f;
		const float xPos = 0.0f;
		const float zPos = 0.0f;
		entityManager->CreatePlayer({
			glm::vec3(
				xPos,
				terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos / terrainXScale, zPos / terrainZScale, false) + scaleFactor * 0.5f,
				zPos
			),
			glm::vec3(scaleFactor),
			glm::vec4(1.0f),
			-1,
		});
	}
	//*/

	//* Create coins
	for(short i = 0; i < 20; ++i){
		const float scaleFactor = 70.f;
		const float xPos = PseudorandMinMax(-terrainXScale * 0.5f + 2.0f, terrainXScale * 0.5f - 2.0f);
		const float zPos = PseudorandMinMax(-terrainZScale * 0.5f + 2.0f, terrainZScale * 0.5f - 2.0f);
		const glm::vec3 pos = glm::vec3(
			xPos,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos / terrainXScale, zPos / terrainZScale, false) + scaleFactor,
			zPos
		);
		entityManager->CreateCoin({
			pos,
			glm::vec3(scaleFactor),
			glm::vec4(1.0f),
			-1,
		});

		ISound* music = soundEngine->play3D("Audio/Music/Spin.mp3", vec3df(pos.x, pos.y, pos.z), true, true, true, ESM_AUTO_DETECT, true);
		if(music){
			music->setMinDistance(3.f);
			music->setVolume(5);

			ISoundEffectControl* soundFX = music->getSoundEffectControl();
			if(!soundFX){
				(void)puts("No soundFX support!\n");
			}
			coinSoundFX.emplace_back(soundFX);

			coinMusic.emplace_back(music);
		} else{
			(void)puts("Failed to init music!\n");
		}
	}
	//*/

	CreateTreesAndCubes();
	CreateDecorations();

	treeLOD.SetDistAndModel(DetailLvl::High, 5000.0f,  models[(int)ModelType::Tree_High]);
	treeLOD.SetDistAndModel(DetailLvl::Medium, 10000.0f,  models[(int)ModelType::Tree_Medium]);
	treeLOD.SetDistAndModel(DetailLvl::Low, 15000.0f, models[(int)ModelType::Tree_Low]);

	entityManager->SetUpRegionsForStationary();
}

void Scene::CreateTreesAndCubes(){
	Model* const treeHigh = models[(int)ModelType::Tree_High];
	treeHigh->ReserveModelMatsForAll(2000);
	treeHigh->ReserveColorsForAll(2000);
	treeHigh->ReserveDiffuseTexIndicesForAll(2000);

	Model* const treeMedium = models[(int)ModelType::Tree_Medium];
	treeMedium->ReserveModelMatsForAll(2000);
	treeMedium->ReserveColorsForAll(2000);
	treeMedium->ReserveDiffuseTexIndicesForAll(2000);

	Model* const treeLow = models[(int)ModelType::Tree_Low];
	treeLow->ReserveModelMatsForAll(2000);
	treeLow->ReserveColorsForAll(2000);
	treeLow->ReserveDiffuseTexIndicesForAll(2000);

	Mesh* const cubeMesh = Meshes::meshes[(int)MeshType::Cube];
	cubeMesh->ReserveModelMats(2000);
	cubeMesh->ReserveColors(2000);
	cubeMesh->ReserveDiffuseTexIndices(2000);

	for(int i = 0; i < 2000; ++i){
		const float xPos = PseudorandMinMax(-terrainXScale * 0.5f + 2.f, terrainXScale * 0.5f - 2.f);
		const float zPos = PseudorandMinMax(-terrainZScale * 0.5f + 2.f, 2.f);
		const glm::vec3 pos = glm::vec3(
			xPos,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos / terrainXScale, zPos / terrainZScale, false),
			zPos
		);

		modelStack.PushModel({
			modelStack.Translate(pos),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3(200.0f))
		});
			const glm::mat4& modelMat = modelStack.GetTopModel();
			const glm::vec3& color = glm::vec3(PseudorandMinMax(0.1f, 1.0f), 0.0f, 0.0f);
			const int diffuseTexIndex = 0;

			treeHigh->AddModelMatForAll(modelMat);
			treeHigh->AddColorForAll(color);
			treeHigh->AddDiffuseTexIndexForAll(diffuseTexIndex);

			treeMedium->AddModelMatForAll(modelMat);
			treeMedium->AddColorForAll(color);
			treeMedium->AddDiffuseTexIndexForAll(diffuseTexIndex);

			treeLow->AddModelMatForAll(modelMat);
			treeLow->AddColorForAll(color);
			treeLow->AddDiffuseTexIndexForAll(diffuseTexIndex);
		modelStack.PopModel();

		modelStack.PushModel({
			modelStack.Translate(pos + glm::vec3(0.0f, (float)PseudorandMinMax(1200, 1600), terrainZScale * 0.5f - 2.f)),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3(40.0f)),
		});
			cubeMesh->AddModelMat(modelStack.GetTopModel());
			cubeMesh->AddColor(glm::vec4(1.0f));
			cubeMesh->AddDiffuseTexIndex(PseudorandMinMax(0, 11));
		modelStack.PopModel();
	}
}

void Scene::CreateDecorations(){
	Model* const flower = models[(int)ModelType::Flower];
	flower->ReserveModelMatsForAll(2000);
	flower->ReserveColorsForAll(2000);
	flower->ReserveDiffuseTexIndicesForAll(2000);

	Model* const grass = models[(int)ModelType::Grass];
	grass->ReserveModelMatsForAll(2000);
	grass->ReserveColorsForAll(2000);
	grass->ReserveDiffuseTexIndicesForAll(2000);

	Model* const rock = models[(int)ModelType::Rock];
	rock->ReserveModelMatsForAll(2000);
	rock->ReserveColorsForAll(2000);
	rock->ReserveDiffuseTexIndicesForAll(2000);

	for(int i = 0; i < 2000; ++i){
		const float xPos0 = PseudorandMinMax(-terrainXScale * 0.5f + 2.f, terrainXScale * 0.5f - 2.f);
		const float zPos0 = PseudorandMinMax(2.f, terrainZScale * 0.5f - 2.f);
		const glm::vec3 pos0 = glm::vec3(
			xPos0,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos0 / terrainXScale, zPos0 / terrainZScale, false),
			zPos0
		);

		const float xPos1 = PseudorandMinMax(-terrainXScale * 0.5f + 2.f, terrainXScale * 0.5f - 2.f);
		const float zPos1 = PseudorandMinMax(2.f, terrainZScale * 0.5f - 2.f);
		const glm::vec3 pos1 = glm::vec3(
			xPos1,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos1 / terrainXScale, zPos1 / terrainZScale, false),
			zPos1
		);

		const float xPos2 = PseudorandMinMax(-terrainXScale * 0.5f + 2.f, terrainXScale * 0.5f - 2.f);
		const float zPos2 = PseudorandMinMax(2.f, terrainZScale * 0.5f - 2.f);
		const glm::vec3 pos2 = glm::vec3(
			xPos2,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos2 / terrainXScale, zPos2 / terrainZScale, false),
			zPos2
		);

		const glm::vec3 brightColor = glm::rgbColor(glm::vec3(PseudorandMinMax(0.1f, 1.0f), PseudorandMinMax(0.1f, 1.0f), 1.0f));

		modelStack.PushModel({
			modelStack.Translate(pos0),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3(140.0f))
		});
			flower->AddModelMatForAll(modelStack.GetTopModel());
			flower->AddColorForAll(brightColor);
			flower->AddDiffuseTexIndexForAll(-1);
		modelStack.PopModel();

		const glm::vec3 normalColor = glm::vec3(PseudorandMinMax(0.1f, 1.0f), PseudorandMinMax(0.1f, 1.0f), PseudorandMinMax(0.1f, 1.0f));

		modelStack.PushModel({
			modelStack.Translate(pos1),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3(140.0f))
		});
			grass->AddModelMatForAll(modelStack.GetTopModel());
			grass->AddColorForAll(normalColor);
			grass->AddDiffuseTexIndexForAll(-1);
		modelStack.PopModel();

		modelStack.PushModel({
			modelStack.Translate(pos2),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3((float)PseudorandMinMax(60, 90)))
		});
			rock->AddModelMatForAll(modelStack.GetTopModel());
			rock->AddColorForAll(normalColor);
			rock->AddDiffuseTexIndexForAll(-1);
		modelStack.PopModel();
	}
}

bool Scene::Init(){
	glGetIntegerv(GL_POLYGON_MODE, polyModes);

	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS & ~ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER & ~ESEO_PRINT_DEBUG_INFO_TO_STDOUT);
	if(!soundEngine){
		(void)puts("Failed to init soundEngine!\n");
	}
	soundEngine->play2D("Audio/Music/Theme.mp3", true);

	InitEntities();

	const std::vector<cstr> faces{
		"Imgs/Skybox/Right.png",
		"Imgs/Skybox/Left.png",
		"Imgs/Skybox/Top.png",
		"Imgs/Skybox/Bottom.png",
		"Imgs/Skybox/Front.png",
		"Imgs/Skybox/Back.png"
	};
	SetUpCubemap(cubemapRefID, faces);

	Meshes::meshes[(int)MeshType::CoinSpriteAni]->AddTexMap({"Imgs/Coin.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::CoinSpriteAni])->AddAni("CoinSpriteAni", 0, 6);
	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::CoinSpriteAni])->Play("CoinSpriteAni", -1, .5f);

	Meshes::meshes[(int)MeshType::FireSpriteAni]->AddTexMap({"Imgs/Fire.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::FireSpriteAni])->AddAni("FireSpriteAni", 0, 32);
	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::FireSpriteAni])->Play("FireSpriteAni", -1, .5f);

	Meshes::meshes[(int)MeshType::Terrain]->AddTexMap({"Imgs/Floor.jpg", Mesh::TexType::Diffuse, 0});

	dLightFromTop = CreateLight(LightType::Directional);
	dLightFromBottom = CreateLight(LightType::Directional);

	dLightFromTop->ambient = glm::vec3(0.3f);
	static_cast<DirectionalLight*>(dLightFromBottom)->dir = glm::vec3(0.0f, 1.0f, 0.0f);
	dLightFromBottom->diffuse = glm::vec3(0.5f, 0.0f, 0.0f);

	return true;
}

void Scene::Update(GLFWwindow* const& win){
	elapsedTime += dt;
	if(winHeight){ //Avoid division by 0 when win is minimised
		cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
		cam.ResetAspectRatio();
	}

	POINT mousePos;
	if(GetCursorPos(&mousePos)){
		HWND hwnd = ::GetActiveWindow();
		(void)ScreenToClient(hwnd, &mousePos);
	} else{
		(void)puts("Failed to get mouse pos relative to screen!");
	}
	static float buttonBT = 0.f;

	switch(screen){
		case Screen::MainMenu:
			MainMenuUpdate(win, mousePos, buttonBT);
			break;
		case Screen::Game:
			GameUpdate(win);
			break;
	}
}

void Scene::MainMenuUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT){
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);

	if(mousePos.x >= (float)winWidth * 0.47f
		&& mousePos.x <= (float)winWidth * 0.53f
		&& mousePos.y >= (float)winHeight * 0.77f
		&& mousePos.y <= (float)winHeight * 0.83f){
		if(textScaleFactors[0] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[0] = 1.1f;
			textColours[0] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);

			if(guns[0]){
				delete guns[0];
				guns[0] = nullptr;
			}
			guns[0] = new Shotgun();
			if(guns[1]){
				delete guns[1];
				guns[1] = nullptr;
			}
			guns[1] = new Scar();
			if(guns[2]){
				delete guns[2];
				guns[2] = nullptr;
			}
			guns[2] = new Sniper();

			screen = Screen::Game;

			cam.SetPos(glm::vec3(0.0f, 1500.0f, 2400.0f));
			cam.SetTarget(glm::vec3(0.0f, 1500.0f, 0.0f));
			cam.SetUp(glm::vec3(0.f, 1.f, 0.f));

			buttonBT = elapsedTime + .3f;

			const size_t& coinMusicSize = coinMusic.size();
			for(size_t i = 0; i < coinMusicSize; ++i){
				ISound* music = coinMusic[i];
				if(music && music->getIsPaused()){
					music->setIsPaused(false);
				}
			}
			const size_t& fireMusicSize = fireMusic.size();
			for(size_t i = 0; i < fireMusicSize; ++i){
				ISound* music = fireMusic[i];
				if(music && music->getIsPaused()){
					music->setIsPaused(false);
				}
			}
		}
	} else{
		textScaleFactors[0] = 1.f;
		textColours[0] = glm::vec4(1.f);
	}

	if(mousePos.x >= (float)winWidth * 0.47f
		&& mousePos.x <= (float)winWidth * 0.53f
		&& mousePos.y >= (float)winHeight * 0.87f
		&& mousePos.y <= (float)winHeight * 0.93f){
		if(textScaleFactors[1] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[1] = 1.1f;
			textColours[1] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);
			endLoop = true;
			buttonBT = elapsedTime + .3f;
		}
	} else{
		textScaleFactors[1] = 1.f;
		textColours[1] = glm::vec4(1.f);
	}
}

void Scene::GameUpdate(GLFWwindow* const& win){
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	static float camAttachmentBT = 0.0f;
	if(Key(GLFW_KEY_B) && camAttachmentBT <= elapsedTime){
		isCamDetached = !isCamDetached;
		camAttachmentBT = elapsedTime + .5f;
	}

	if(isCamDetached){
		cam.UpdateDetached(GLFW_KEY_E, GLFW_KEY_Q, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	} else{
		//cam.UpdateJumpFall();
		//cam.UpdateAttached(
		//	GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S,
		//	-terrainXScale / 2.f + 5.f, terrainXScale / 2.f - 5.f,
		//	yMin, yMax,
		//	-terrainZScale / 2.f + 5.f, terrainZScale / 2.f - 5.f
		//);
	}

	view = cam.LookAt();

	///Control FOV of perspective projection based on item selected in inv
	if(RMB){
		switch(inv[currSlot]){
			case ItemType::Shotgun:
				angularFOV = 40.f;
				break;
			case ItemType::Scar:
				angularFOV = 30.f;
				break;
			case ItemType::Sniper:
				if(angularFOV != 15.f){
					soundEngine->play2D("Audio/Sounds/Scope.wav", false);
				}
				angularFOV = 15.f;
				break;
		}
	} else{
		angularFOV = 45.f;
	}
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	const glm::vec3& camPos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();
	soundEngine->setListenerPosition(vec3df(camPos.x, camPos.y, camPos.z), vec3df(camFront.x, camFront.y, camFront.z));

	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::CoinSpriteAni])->Update();
	static_cast<SpriteAni*>(Meshes::meshes[(int)MeshType::FireSpriteAni])->Update();

	static float polyModeBT = 0.f;
	static float minimapViewBT = 0.f;
	static float distortionBT = 0.f;
	static float echoBT = 0.f;
	static float wavesReverbBT = 0.f;
	static float resetSoundFXBT = 0.f;

	///Track changing of inv slots
	int keys[]{
		GLFW_KEY_1,
		GLFW_KEY_2,
		GLFW_KEY_3,
		GLFW_KEY_4,
		GLFW_KEY_5,
	};
	for(int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i){
		if(Key(keys[i])){
			currSlot = keys[i] - 49;
			break;
		}
	}
	switch(inv[currSlot]){
		case ItemType::Shotgun:
		case ItemType::Scar:
		case ItemType::Sniper:
			currGun = guns[int(inv[currSlot])];
			break;
		default:
			currGun = nullptr;
	}

	if(Key(VK_F2) && polyModeBT <= elapsedTime){
		polyModes[0] += polyModes[0] == GL_FILL ? -2 : 1;
		glPolygonMode(GL_FRONT_AND_BACK, polyModes[0]);
		polyModeBT = elapsedTime + .5f;
	}

	///Control shooting and reloading of currGun
	if(currGun){
		if(LMB){
			currGun->Shoot(elapsedTime, cam.GetPos(), cam.CalcFront(), soundEngine);
		}
		if(Key(GLFW_KEY_R)){
			currGun->Reload(soundEngine);
		}
		currGun->Update();
	}

	entityManager->Update();
}

void Scene::MainMenuRender(){
	forwardSP.Set1i("nightVision", 0);

	modelStack.PushModel({
		modelStack.Scale(glm::vec3(float(winWidth) / 2.f, float(winHeight) / 2.f, 1.f)),
	});
		forwardSP.Set1i("noNormals", 1);
		Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
		Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
		forwardSP.Set1i("noNormals", 0);
	modelStack.PopModel();

	glDepthFunc(GL_GREATER);

	textChief.RenderText(textSP, {
		"Play",
		(float)winWidth * 0.5f,
		(float)winHeight * 0.2f,
		textScaleFactors[0],
		textColours[0],
		0,
		TextChief::TextAlignment::Center
	});
	textChief.RenderText(textSP, {
		"Quit",
		(float)winWidth * 0.5f, 
		(float)winHeight * 0.1f,
		textScaleFactors[1],
		textColours[1],
		0,
		TextChief::TextAlignment::Center
	});

	textChief.RenderText(textSP, {
		"ANOTHER",
		(float)winWidth * 0.5f, 
		(float)winHeight * 0.8f,
		4.0f,
		glm::vec4(glm::vec3(1.f, 0.f, 1.f), 1.f),
		0,
		TextChief::TextAlignment::Center
	});
	textChief.RenderText(textSP, {
		"WORLD",
		(float)winWidth * 0.5f,
		(float)winHeight * 0.6f,
		4.0f,
		glm::vec4(glm::vec3(1.f, 0.f, 1.f), 1.f),
		0,
		TextChief::TextAlignment::Center
	});

	glDepthFunc(GL_LESS);
}

void Scene::GameRender(){
	const glm::vec3 OGPos = cam.GetPos();
	const glm::vec3 OGTarget = cam.GetTarget();
	const glm::vec3 OGUp = cam.GetUp();

	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);
	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

	if(!(RMB && inv[currSlot] == ItemType::Sniper)){
		///Render items in inv
		for(short i = 0; i < 5; ++i){
			forwardSP.Set1i("noNormals", 1);
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(float(i) * 100.f - 200.f, -float(winHeight) / 2.3f, -10.f)),
			});
			switch(inv[i]){
				case ItemType::Shotgun:
					modelStack.PushModel({
						modelStack.Translate(glm::vec3(18.f, -18.f, 0.f)),
						modelStack.Rotate(glm::vec4(0.f, 0.f, 1.f, 45.f)),
						modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, 90.f)),
						modelStack.Scale(glm::vec3(21.f)),
					});
					models[(int)ModelType::Shotgun]->SetModelForAll(modelStack.GetTopModel());
					models[(int)ModelType::Shotgun]->Render(forwardSP);
					modelStack.PopModel();
					break;
				case ItemType::Scar:
					modelStack.PushModel({
						modelStack.Rotate(glm::vec4(0.f, 0.f, 1.f, 45.f)),
						modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, 90.f)),
						modelStack.Scale(glm::vec3(18.f)),
					});
						models[(int)ModelType::Scar]->SetModelForAll(modelStack.GetTopModel());
						models[(int)ModelType::Scar]->Render(forwardSP);
					modelStack.PopModel();
					break;
				case ItemType::Sniper:
					modelStack.PushModel({
						modelStack.Translate(glm::vec3(16.f, -15.f, 0.f)),
						modelStack.Rotate(glm::vec4(0.f, 0.f, 1.f, 45.f)),
						modelStack.Scale(glm::vec3(10.f)),
					});
						models[(int)ModelType::Sniper]->SetModelForAll(modelStack.GetTopModel());
						models[(int)ModelType::Sniper]->Render(forwardSP);
					modelStack.PopModel();
					break;
				case ItemType::ShotgunAmmo:
					break;
				case ItemType::ScarAmmo:
					break;
				case ItemType::SniperAmmo:
					break;
			}
			modelStack.PopModel();
			forwardSP.Set1i("noNormals", 0);
		}
	}

	cam.SetPos(OGPos);
	cam.SetTarget(OGTarget);
	cam.SetUp(OGUp);
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

	forwardSP.SetMat4fv("PV", &(projection * glm::mat4(glm::mat3(view)))[0][0]);


	Mesh* const cubeMesh = Meshes::meshes[(int)MeshType::Cube];

	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	glCullFace(GL_FRONT);
		forwardSP.Set1i("sky", 1);
		forwardSP.Set1i("skybox", 1);
		forwardSP.UseTex(cubemapRefID, "cubemapSampler", GL_TEXTURE_CUBE_MAP);
		cubeMesh->SetModel(modelStack.GetTopModel());
		cubeMesh->Render(forwardSP);
		forwardSP.Set1i("skybox", 0);
		forwardSP.Set1i("sky", 0);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

	///Render item held
	const glm::vec3 front = cam.CalcFront();
	const float sign = front.y < 0.f ? -1.f : 1.f;

	const glm::mat4 rotationMat = glm::rotate(glm::mat4(1.f), sign * acosf(glm::dot(front, glm::normalize(glm::vec3(front.x, 0.f, front.z)))),
		glm::normalize(glm::vec3(-front.z, 0.f, front.x)));

	switch(inv[currSlot]){
		case ItemType::Shotgun: {
			modelStack.PushModel({
				modelStack.Translate(cam.GetPos() +
					glm::vec3(rotationMat * glm::vec4(RotateVecIn2D(glm::vec3(5.5f, -7.f, -13.f), atan2(front.x, front.z) + glm::radians(180.f), Axis::y), 1.f))
			),
				modelStack.Rotate(glm::vec4(glm::vec3(-front.z, 0.f, front.x), sign * glm::degrees(acosf(glm::dot(front, glm::normalize(glm::vec3(front.x, 0.f, front.z))))))), 
				modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(front.x, front.z)))), 
				modelStack.Scale(glm::vec3(3.f)),
			});
				models[(int)ModelType::Shotgun]->SetModelForAll(modelStack.GetTopModel());
				models[(int)ModelType::Shotgun]->Render(forwardSP);
			modelStack.PopModel();
			break;
		}
		case ItemType::Scar: {
			modelStack.PushModel({
				modelStack.Translate(cam.GetPos() +
					glm::vec3(rotationMat * glm::vec4(RotateVecIn2D(glm::vec3(5.f, -4.f, -12.f), atan2(front.x, front.z) + glm::radians(180.f), Axis::y), 1.f))
			),
				modelStack.Rotate(glm::vec4(glm::vec3(-front.z, 0.f, front.x), sign * glm::degrees(acosf(glm::dot(front, glm::normalize(glm::vec3(front.x, 0.f, front.z))))))), 
				modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(front.x, front.z)))), 
				modelStack.Scale(glm::vec3(3.f)),
			});
				models[(int)ModelType::Scar]->SetModelForAll(modelStack.GetTopModel());
				models[(int)ModelType::Scar]->Render(forwardSP);
				modelStack.PopModel();
			break;
		}
		case ItemType::Sniper: {
			modelStack.PushModel({
				modelStack.Translate(cam.GetPos() +
				glm::vec3(rotationMat * glm::vec4(RotateVecIn2D(glm::vec3(5.f, -6.f, -13.f), atan2(front.x, front.z) + glm::radians(180.f), Axis::y), 1.f))
			),
				modelStack.Rotate(glm::vec4(glm::vec3(-front.z, 0.f, front.x), sign * glm::degrees(acosf(glm::dot(front, glm::normalize(glm::vec3(front.x, 0.f, front.z))))))), 
				modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(front.x, front.z)) - 90.f)), 
				modelStack.Scale(glm::vec3(2.f)),
			});
				models[(int)ModelType::Sniper]->SetModelForAll(modelStack.GetTopModel());
				models[(int)ModelType::Sniper]->Render(forwardSP);
				modelStack.PopModel();
			break;
		}
	}

	///Terrain
	modelStack.PushModel({
		modelStack.Scale(glm::vec3(terrainXScale, terrainYScale, terrainZScale)),
	});
		Meshes::meshes[(int)MeshType::Terrain]->SetModel(modelStack.GetTopModel());
		Meshes::meshes[(int)MeshType::Terrain]->Render(forwardSP);
	modelStack.PopModel();




	cubeMesh->InstancedRender(forwardSP);
	treeLOD.GetModel(glm::length(cam.GetPos() - glm::vec3()))->InstancedRender(forwardSP); //playerPos?? //lenSquared??
	models[(int)ModelType::Flower]->InstancedRender(forwardSP);
	models[(int)ModelType::Grass]->InstancedRender(forwardSP);
	models[(int)ModelType::Rock]->InstancedRender(forwardSP);

	entityManager->Render(forwardSP, cam);




	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);
	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

	Mesh* const quadMesh = Meshes::meshes[(int)MeshType::Quad];

	if(RMB && inv[currSlot] == ItemType::Sniper){
		forwardSP.Set1i("nightVision", 1);

		forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
		forwardSP.Set1i("noNormals", 1);

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(0.f, 0.f, -9.f)),
			modelStack.Scale(glm::vec3(float(winHeight) * 0.7f, float(winHeight) * 0.7f, 1.f)),
		});
			forwardSP.Set1i("customDiffuseTexIndex", 1);
			quadMesh->SetModel(modelStack.GetTopModel());
			quadMesh->Render(forwardSP);
		modelStack.PopModel();
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(0.f, 0.f, -9.1f)),
			modelStack.Scale(glm::vec3(float(winWidth) / 2.f, float(winHeight) / 2.f, 1.f)),
		});
			forwardSP.Set1i("customDiffuseTexIndex", -1);
			forwardSP.Set1i("useCustomColour", 1);
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.f), 1.f));
			quadMesh->SetModel(modelStack.GetTopModel());
			quadMesh->Render(forwardSP);
			forwardSP.Set1i("useCustomColour", 0);
		modelStack.PopModel();

		forwardSP.Set1i("noNormals", 0);
		forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
	} else{
		forwardSP.Set1i("nightVision", 0);

		///Render inv slots
		for(short i = 0; i < 5; ++i){
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(float(i) * 100.f - 200.f, -float(winHeight) / 2.3f, -10.f)),
				modelStack.Scale(glm::vec3(50.f, 50.f, 1.f)),
			});
				forwardSP.Set1i("noNormals", 1);
				if(i == currSlot){
					forwardSP.Set1i("useCustomColour", 1);
					forwardSP.Set4fv("customColour", glm::vec4(1.f, 1.f, 0.f, 1.f));
				}
				forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
				forwardSP.Set1i("customDiffuseTexIndex", 3);
				quadMesh->SetModel(modelStack.GetTopModel());
				quadMesh->Render(forwardSP);
				forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
				if(i == currSlot){
					forwardSP.Set1i("useCustomColour", 0);
				}
				forwardSP.Set1i("noNormals", 0);
			modelStack.PopModel();
		}

		///Render reticle
		if(inv[currSlot] == ItemType::Shotgun || inv[currSlot] == ItemType::Scar){
			modelStack.PushModel({
				modelStack.Scale(glm::vec3(40.f, 40.f, 1.f)),
			});
				forwardSP.Set1i("noNormals", 1);
				forwardSP.Set1i("useCustomColour", 1);
				forwardSP.Set4fv("customColour", reticleColour);
				forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
				forwardSP.Set1i("customDiffuseTexIndex", 4);
				quadMesh->SetModel(modelStack.GetTopModel());
				quadMesh->Render(forwardSP);
				forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
				forwardSP.Set1i("useCustomColour", 0);
				forwardSP.Set1i("noNormals", 0);

				modelStack.PushModel({
					modelStack.Translate(glm::vec3(0.f, 0.f, 1.f)),
				});
				if(RMB){
					modelStack.PushModel({
						modelStack.Scale(glm::vec3(.7f, .7f, 1.f)),
					});
				}
					forwardSP.Set1i("noNormals", 1);
					forwardSP.Set1i("useCustomColour", 1);
					forwardSP.Set4fv("customColour", reticleColour);
					forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
					forwardSP.Set1i("customDiffuseTexIndex", inv[currSlot] == ItemType::Shotgun ? 5 : 6);
					quadMesh->SetModel(modelStack.GetTopModel());
					quadMesh->Render(forwardSP);
					forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
					forwardSP.Set1i("useCustomColour", 0);
					forwardSP.Set1i("noNormals", 0);
				modelStack.PopModel();
				modelStack.PopModel();
			modelStack.PopModel();
		}
	}

	cam.SetPos(OGPos);
	cam.SetTarget(OGTarget);
	cam.SetUp(OGUp);
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);
	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

	///Render bullet info
	if(currGun){
		textChief.RenderText(textSP, {
			std::to_string(currGun->GetLoadedBullets()) + " / " + std::to_string(currGun->GetUnloadedBullets()),
			float(winWidth) * 0.5f,
			float(winHeight) * 0.15f,
			1.f,
			glm::vec4(1.f, 1.f, 0.f, 1.f),
			0,
			TextChief::TextAlignment::Center
		});
	}

	const float FPS = 1.0f / dt;
	textChief.RenderText(textSP, {
		"FPS: " + std::to_string(FPS).substr(0, std::to_string((int)FPS).length() + 3),
		25.0f,
		(float)winHeight * 0.05f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	textChief.RenderText(textSP, {
		"Instanced draw calls: " + std::to_string(Mesh::instancedDrawCalls),
		25.f,
		(float)winHeight * 0.2f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	textChief.RenderText(textSP, {
		"Normal draw calls: " + std::to_string(Mesh::normalDrawCalls),
		25.f,
		(float)winHeight * 0.25f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	textChief.RenderText(textSP, {
		"Polygon count: " + std::to_string(Mesh::polygonCount),
		25.f,
		(float)winHeight * 0.3f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	textChief.RenderText(textSP, {
		"Index count: " + std::to_string(Mesh::indexCount),
		25.f,
		(float)winHeight * 0.35f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	textChief.RenderText(textSP, {
		"Vertex count: " + std::to_string(Mesh::vertexCount),
		25.f,
		(float)winHeight * 0.4f,
		1.f,
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		0,
	});

	Mesh::instancedDrawCalls = 0;
	Mesh::normalDrawCalls = 0;
	Mesh::vertexCount = 0;
	Mesh::indexCount = 0;
	Mesh::polygonCount = 0;
}

void Scene::ForwardRender(){
	forwardSP.Use();

	forwardSP.Set1f("shininess", 32.f); //More light scattering if lower
	forwardSP.Set3fv("globalAmbient", Light::globalAmbient);
	forwardSP.Set3fv("camPos", cam.GetPos());
	forwardSP.Set1i("dAmt", 2);

	DirectionalLight* directionalLightFromTop = (DirectionalLight*)dLightFromTop;
	forwardSP.Set3fv("directionalLights[0].ambient", directionalLightFromTop->ambient);
	forwardSP.Set3fv("directionalLights[0].diffuse", directionalLightFromTop->diffuse);
	forwardSP.Set3fv("directionalLights[0].spec", directionalLightFromTop->spec);
	forwardSP.Set3fv("directionalLights[0].dir", directionalLightFromTop->dir);

	DirectionalLight* directionalLightFromBottom = (DirectionalLight*)dLightFromBottom;
	forwardSP.Set3fv("directionalLights[1].ambient", directionalLightFromBottom->ambient);
	forwardSP.Set3fv("directionalLights[1].diffuse", directionalLightFromBottom->diffuse);
	forwardSP.Set3fv("directionalLights[1].spec", directionalLightFromBottom->spec);
	forwardSP.Set3fv("directionalLights[1].dir", directionalLightFromBottom->dir);

	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch(screen){
		case Screen::MainMenu:
			MainMenuRender();
			break;
		case Screen::Game:
			GameRender();
			break;
	}

	glBlendFunc(GL_ONE, GL_ZERO);
}