#include "Scene.h"
#include "Vendor/stb_image.h"
#include <glm/gtx/color_space.hpp>

float terrainXScale = 1500.f;
float terrainYScale = 200.f;
float terrainZScale = 1500.f;

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
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f), 0.f, 150.f),
	minimapCam(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), 0.f, 0.f),
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
		new Model("ObjsAndMtls/Tree.obj", {
			aiTextureType_DIFFUSE,
		}),
	},
	blurSP{"Shaders/Quad.vertexS", "Shaders/Blur.fragS"},
	forwardSP{"Shaders/Forward.vertexS", "Shaders/Forward.fragS"},
	normalsSP{"Shaders/Normals.vertexS", "Shaders/Normals.fragS", "Shaders/Normals.gs"},
	screenSP{"Shaders/Quad.vertexS", "Shaders/Screen.fragS"},
	textSP{"Shaders/Text.vertexS", "Shaders/Text.fragS"},
	ptLights({}),
	directionalLights({}),
	spotlights({}),
	cubemapRefID(0),
	minimapView(MinimapViewType::TopFollowingOrtho),
	currSlot(0),
	inv{
		ItemType::Shotgun,
		ItemType::Scar,
		ItemType::Sniper,
		ItemType::None,
		ItemType::None,
	},
	playerStates((int)PlayerState::NoMovement | (int)PlayerState::Standing),
	screen(Screen::MainMenu),
	timeLeft(0.f),
	score(0),
	scores({}),
	textScaleFactors{
		1.f,
		1.f,
		1.f,
	},
	textColours{
		glm::vec4(1.f),
		glm::vec4(1.f),
		glm::vec4(1.f),
	},
	sprintOn(false),
	playerHealth(0.f),
	playerMaxHealth(100.f),
	playerLives(0),
	playerMaxLives(5),
	currGun(nullptr),
	guns{
		nullptr,
		nullptr,
		nullptr,
	},
	reticleColour(glm::vec4(1.f)),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	isCamDetached(false),
	elapsedTime(0.f),
	modelStack(),
	polyModes(),
	entityManager(EntityManager::GetObjPtr())
{
}

Scene::~Scene(){
	///Create save
	str line;
	try{
		std::ofstream stream("Data/scores.dat", std::ios::out);
		if(stream.is_open()){
			const size_t& mySize = scores.size();
			for(size_t i = 0; i < mySize; ++i){
				stream << (!i ? "" : "\n") + std::to_string(scores[i]);
			}
			stream.close();
		} else{
			throw("Failed to save scores!");
		}
	} catch(cstr const& errorMsg){
		(void)puts(errorMsg);
	}

	for(short i = 0; i < 3; ++i){
		if(guns[i]){
			delete guns[i];
			guns[i] = nullptr;
		}
	}

	const size_t& pSize = ptLights.size();
	const size_t& dSize = directionalLights.size();
	const size_t& sSize = spotlights.size();
	for(size_t i = 0; i < pSize; ++i){
		delete ptLights[i];
		ptLights[i] = nullptr;
	}
	for(size_t i = 0; i < dSize; ++i){
		delete directionalLights[i];
		directionalLights[i] = nullptr;
	}
	for(size_t i = 0; i < sSize; ++i){
		delete spotlights[i];
		spotlights[i] = nullptr;
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

	for(short i = 0; i < 20; ++i){
		const float scaleFactor = 15.f;
		const float xPos = PseudorandMinMax(-terrainXScale * 0.5f + 5.f, terrainXScale * 0.5f - 5.f);
		const float zPos = PseudorandMinMax(-terrainZScale * 0.5f + 5.f, terrainZScale * 0.5f - 5.f);
		const glm::vec3 pos = glm::vec3(xPos, terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos / terrainXScale, zPos / terrainZScale) + scaleFactor, zPos);
		entityManager->CreateCoin({
			pos,
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(scaleFactor),
			glm::vec4(5.f * glm::rgbColor(glm::vec3(PseudorandMinMax(0.f, 255.f), 1.f, 1.f)), .99f),
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

	//* Create trees
	Model* const tree = models[(int)ModelType::Tree];
	tree->ReserveModelMatsForAll(400);
	tree->ReserveColorsForAll(400);
	tree->ReserveDiffuseTexIndicesForAll(400);

	for(int i = 0; i < 400; ++i){
		const float scaleFactor = 50.0f;
		const float xPos = PseudorandMinMax(-terrainXScale * 0.5f + 2.f, terrainXScale * 0.5f - 2.f);
		const float zPos = PseudorandMinMax(-terrainZScale * 0.5f + 2.f, terrainZScale * 0.5f - 2.f);
		const glm::vec3 pos = glm::vec3(
			xPos,
			terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(xPos / terrainXScale, zPos / terrainZScale, false),
			zPos
		);

		modelStack.PushModel({
			modelStack.Translate(pos),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, PseudorandMinMax(0.0f, 360.0f))),
			modelStack.Scale(glm::vec3(scaleFactor))
		});
			tree->AddModelMatForAll(modelStack.GetTopModel());
			tree->AddColorForAll(glm::rgbColor(glm::vec3(PseudorandMinMax(0.0f, 1.0f), 1.0f, PseudorandMinMax(0.0f, 1.0f))));
			tree->AddDiffuseTexIndexForAll(0);
		modelStack.PopModel();
	}
	//*/

	entityManager->SetUpRegionsForStationary();
}

bool Scene::Init(){
	glGetIntegerv(GL_POLYGON_MODE, polyModes);

	soundEngine = createIrrKlangDevice(ESOD_AUTO_DETECT, ESEO_DEFAULT_OPTIONS & ~ESEO_PRINT_DEBUG_INFO_TO_DEBUGGER & ~ESEO_PRINT_DEBUG_INFO_TO_STDOUT);
	if(!soundEngine){
		(void)puts("Failed to init soundEngine!\n");
	}
	soundEngine->play2D("Audio/Music/Theme.mp3", true);

	InitEntities();

	///Load save
	cstr const& fPath = "Data/scores.dat";
	str line;
	std::ifstream stream(fPath, std::ios::in);
	if(stream.is_open()){
		while(getline(stream, line)){
			try{
				scores.emplace_back(stoi(line));
			} catch(const std::invalid_argument& e){
				(void)puts(e.what());
			}
		}
		stream.close();
	}
	if(scores.size() > 1){
		std::sort(scores.begin(), scores.end(), std::greater<int>());
	}

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

	directionalLights.emplace_back(CreateLight(LightType::Directional)); //Simulate sunlight

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
		case Screen::GameOver:
		case Screen::MainMenu:
			MainMenuAndGameOverUpdate(win, mousePos, buttonBT);
			break;
		case Screen::Game:
			GameUpdate(win);
			break;
		case Screen::Scoreboard:
			ScoreboardUpdate(win, mousePos, buttonBT);
			break;
	}
}

void Scene::MainMenuAndGameOverUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT){
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);

	if(mousePos.x >= 25.f && mousePos.x <= (screen == Screen::MainMenu ? 100.f : 230.f) && mousePos.y >= winHeight - 160.f && mousePos.y <= winHeight - 125.f){
		if(textScaleFactors[0] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[0] = 1.1f;
			textColours[0] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);
			timeLeft = 60.f;
			score = 0;
			playerHealth = playerMaxHealth;
			playerLives = playerMaxLives;
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
			cam.SetPos(glm::vec3(0.f));
			cam.SetTarget(glm::vec3(0.f, 0.f, -1.f));
			cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
			buttonBT = elapsedTime + .3f;
		}
	} else{
		textScaleFactors[0] = 1.f;
		textColours[0] = glm::vec4(1.f);
	}
	if(mousePos.x >= 25.f && mousePos.x <= 230.f && mousePos.y >= winHeight - 110.f && mousePos.y <= winHeight - 75.f){
		if(textScaleFactors[1] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[1] = 1.1f;
			textColours[1] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);
			screen = Screen::Scoreboard;
			buttonBT = elapsedTime + .3f;
		}
	} else{
		textScaleFactors[1] = 1.f;
		textColours[1] = glm::vec4(1.f);
	}
	if(mousePos.x >= 25.f && mousePos.x <= 100.f && mousePos.y >= winHeight - 60.f && mousePos.y <= winHeight - 25.f){
		if(textScaleFactors[2] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[2] = 1.1f;
			textColours[2] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);
			endLoop = true;
			buttonBT = elapsedTime + .3f;
		}
	} else{
		textScaleFactors[2] = 1.f;
		textColours[2] = glm::vec4(1.f);
	}
}

void Scene::GameUpdate(GLFWwindow* const& win){
	if(playerHealth <= 0.f){
		--playerLives;
		playerHealth = playerMaxHealth;
	}
	if(playerLives <= 0){
		timeLeft = 0.f;
	} else{
		timeLeft -= dt;
	}
	if(timeLeft <= 0.f){
		screen = Screen::GameOver;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		const size_t& mySize = scores.size();
		if(mySize == 5){ //Max no. of scores saved
			std::sort(scores.begin(), scores.end(), std::greater<int>());
			if(score > scores.back()){
				scores.pop_back();
				scores.emplace_back(score);
			}
			std::sort(scores.begin(), scores.end(), std::greater<int>());
		} else{
			scores.emplace_back(score);
			std::sort(scores.begin(), scores.end(), std::greater<int>());
		}
	}
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	static float camAttachmentBT = 0.0f;
	if(Key(GLFW_KEY_B) && camAttachmentBT <= elapsedTime){
		isCamDetached = !isCamDetached;
		camAttachmentBT = elapsedTime + .5f;
	}

	if(isCamDetached){
		cam.SetVel(150.f);
		cam.UpdateDetached(GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	} else{
		////Control player states
		static float sprintBT = 0.f;
		static float heightBT = 0.f;

		///Toggle sprint
		if(Key(VK_SHIFT) && sprintBT <= elapsedTime){
			sprintOn = !sprintOn;
			sprintBT = elapsedTime + .5f;
		}

		///Set movement state
		if(Key(GLFW_KEY_A) || Key(GLFW_KEY_D) || Key(GLFW_KEY_W) || Key(GLFW_KEY_S)){
			if(sprintOn){
				playerStates &= ~(int)PlayerState::NoMovement;
				playerStates &= ~(int)PlayerState::Walking;
				playerStates |= (int)PlayerState::Sprinting;
			} else{
				playerStates &= ~(int)PlayerState::NoMovement;
				playerStates |= (int)PlayerState::Walking;
				playerStates &= ~(int)PlayerState::Sprinting;
			}
		} else{
			playerStates |= (int)PlayerState::NoMovement;
			playerStates &= ~(int)PlayerState::Walking;
			playerStates &= ~(int)PlayerState::Sprinting;
		}

		///Set height state
		if(heightBT <= elapsedTime){
			if(Key(GLFW_KEY_C)){
				if(playerStates & (int)PlayerState::Standing){
					playerStates |= (int)PlayerState::Crouching;
					playerStates &= ~(int)PlayerState::Standing;
				} else if(playerStates & (int)PlayerState::Crouching){
					playerStates |= (int)PlayerState::Proning;
					playerStates &= ~(int)PlayerState::Crouching;
				}
				heightBT = elapsedTime + .5f;
			}
			if(Key(VK_SPACE)){
				if(playerStates & (int)PlayerState::Proning){
					playerStates |= (int)PlayerState::Crouching;
					playerStates &= ~(int)PlayerState::Proning;
				} else if(playerStates & (int)PlayerState::Crouching){
					playerStates |= (int)PlayerState::Standing;
					playerStates &= ~(int)PlayerState::Crouching;
				} else if(playerStates & (int)PlayerState::Standing){
					soundEngine->play2D("Audio/Sounds/Jump.wav", false);
					playerStates |= (int)PlayerState::Jumping;
					playerStates &= ~(int)PlayerState::Standing;
				}
				heightBT = elapsedTime + .5f;
			} else{
				if((playerStates & (int)PlayerState::Jumping)){
					playerStates |= (int)PlayerState::Falling;
					playerStates &= ~(int)PlayerState::Jumping;
					cam.SetVel(0.f);
				}
			}
		}

		float yMin = terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(cam.GetPos().x / terrainXScale, cam.GetPos().z / terrainZScale);
		float yMax = yMin;

		///Update player according to its states
		int playerStatesTemp = playerStates;
		int bitMask = 1;
		while(playerStatesTemp){
			switch(PlayerState(playerStatesTemp & bitMask)){
				case PlayerState::NoMovement:
					cam.SetSpd(0.f);
					break;
				case PlayerState::Walking:
					cam.SetSpd(100.f);
					break;
				case PlayerState::Sprinting:
					cam.SetSpd(250.f);
					break;
				case PlayerState::Standing:
					yMin += 30.f;
					yMax += 30.f;
					break;
				case PlayerState::Jumping:
					cam.SetVel(300.f);
				case PlayerState::Falling:
					cam.SetAccel(-1500.f);
					yMin += 30.f;
					yMax += 250.f;
					break;
				case PlayerState::Crouching:
					cam.SetSpd(cam.GetSpd() / 5.f);
					yMin += 5.f;
					yMax += 5.f;
					break;
				case PlayerState::Proning:
					cam.SetSpd(5.f);
					yMin += 1.f;
					yMax += 1.f;
					break;
			}
			playerStatesTemp &= ~bitMask;
			bitMask <<= 1;
		}

		if(playerStates & (int)PlayerState::Jumping){
			if(cam.GetPos().y >= yMax){
				playerStates |= (int)PlayerState::Falling;
				playerStates &= ~(int)PlayerState::Jumping;
				cam.SetVel(0.f);
			}
		}
		if(playerStates & (int)PlayerState::Falling){
			if(cam.GetPos().y <= yMin){
				playerStates |= (int)PlayerState::Standing;
				playerStates &= ~(int)PlayerState::Falling;
				cam.SetAccel(0.f);
				cam.SetVel(0.f);
			}
		}

		cam.UpdateJumpFall();
		cam.UpdateAttached(
			GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S,
			-terrainXScale / 2.f + 5.f, terrainXScale / 2.f - 5.f,
			yMin, yMax,
			-terrainZScale / 2.f + 5.f, terrainZScale / 2.f - 5.f
		);
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

	if(Key(VK_F3) && minimapViewBT <= elapsedTime){
		minimapView = MinimapViewType((int)minimapView + 1);
		if(minimapView == MinimapViewType::Amt){
			minimapView = MinimapViewType(0);
		}
		minimapViewBT = elapsedTime + .5f;
	}

	if(score < 0){
		score = 0;
	}

	///Control soundFX
	const size_t& coinSoundFXSize = coinSoundFX.size();
	for(size_t i = 0; i < coinSoundFXSize; ++i){
		ISoundEffectControl* soundFX = coinSoundFX[i];
		if(soundFX){
			if(Key(KEY_I) && distortionBT <= elapsedTime){
				soundFX->isDistortionSoundEffectEnabled() ? soundFX->disableDistortionSoundEffect() : (void)soundFX->enableDistortionSoundEffect();
				distortionBT = elapsedTime + .5f;
			}
			if(Key(KEY_O) && echoBT <= elapsedTime){
				soundFX->isEchoSoundEffectEnabled() ? soundFX->disableEchoSoundEffect() : (void)soundFX->enableEchoSoundEffect();
				echoBT = elapsedTime + .5f;
			}
			if(Key(KEY_P) && wavesReverbBT <= elapsedTime){
				soundFX->isWavesReverbSoundEffectEnabled() ? soundFX->disableWavesReverbSoundEffect() : (void)soundFX->enableWavesReverbSoundEffect();
				wavesReverbBT = elapsedTime + .5f;
			}
			if(Key(KEY_L) && resetSoundFXBT <= elapsedTime){
				soundFX->disableAllEffects();
				resetSoundFXBT = elapsedTime + .5f;
			}
		}
	}
	const size_t& fireSoundFXSize = fireSoundFX.size();
	for(size_t i = 0; i < fireSoundFXSize; ++i){
		ISoundEffectControl* soundFX = fireSoundFX[i];
		if(soundFX){
			if(Key(KEY_I) && distortionBT <= elapsedTime){
				soundFX->isDistortionSoundEffectEnabled() ? soundFX->disableDistortionSoundEffect() : (void)soundFX->enableDistortionSoundEffect();
				distortionBT = elapsedTime + .5f;
			}
			if(Key(KEY_O) && echoBT <= elapsedTime){
				soundFX->isEchoSoundEffectEnabled() ? soundFX->disableEchoSoundEffect() : (void)soundFX->enableEchoSoundEffect();
				echoBT = elapsedTime + .5f;
			}
			if(Key(KEY_P) && wavesReverbBT <= elapsedTime){
				soundFX->isWavesReverbSoundEffectEnabled() ? soundFX->disableWavesReverbSoundEffect() : (void)soundFX->enableWavesReverbSoundEffect();
				wavesReverbBT = elapsedTime + .5f;
			}
			if(Key(KEY_L) && resetSoundFXBT <= elapsedTime){
				soundFX->disableAllEffects();
				resetSoundFXBT = elapsedTime + .5f;
			}
		}
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

void Scene::ScoreboardUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT){
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);

	if(mousePos.x >= 25.f && mousePos.x <= 110.f && mousePos.y >= winHeight - 60.f && mousePos.y <= winHeight - 25.f){
		if(textScaleFactors[2] != 1.1f){
			soundEngine->play2D("Audio/Sounds/Pop.flac", false);
			textScaleFactors[2] = 1.1f;
			textColours[2] = glm::vec4(1.f, 1.f, 0.f, 1.f);
		}
		if(leftRightMB > 0.f && buttonBT <= elapsedTime){
			soundEngine->play2D("Audio/Sounds/Select.wav", false);
			screen = Screen::MainMenu;
			buttonBT = elapsedTime + .3f;
		}
	} else{
		textScaleFactors[2] = 1.f;
		textColours[2] = glm::vec4(1.f);
	}
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
		25.f,
		125.f,
		textScaleFactors[0],
		textColours[0],
		0,
	});
	textChief.RenderText(textSP, {
		"Scores",
		25.f,
		75.f,
		textScaleFactors[1],
		textColours[1],
		0,
	});
	textChief.RenderText(textSP, {
		"Quit",
		25.f,
		25.f,
		textScaleFactors[2],
		textColours[2],
		0,
	});
	textChief.RenderText(textSP, {
		"ANOTHER",
		30.f,
		float(winHeight) / 1.1f,
		2.f,
		glm::vec4(glm::vec3(1.f, 0.f, 1.f), 1.f),
		0,
	});
	textChief.RenderText(textSP, {
		"WORLD",
		30.f,
		float(winHeight) / 1.1f - 100.f,
		2.f,
		glm::vec4(glm::vec3(1.f, 0.f, 1.f), 1.f),
		0,
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
		///Render healthbar
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(-float(winWidth) / 2.5f, float(winHeight) / 2.5f, -10.f)),
			modelStack.Scale(glm::vec3(float(winWidth) / 15.f, float(winHeight) / 50.f, 1.f)),
		});
		forwardSP.Set1i("noNormals", 1);
		forwardSP.Set1i("useCustomColour", 1);
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.f, 0.f, 0.f), 1.f));
		forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
		forwardSP.Set1i("customDiffuseTexIndex", -1);
		Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
		Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
		forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
		forwardSP.Set1i("useCustomColour", 0);
		forwardSP.Set1i("noNormals", 0);

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(-(playerMaxHealth - playerHealth) / playerMaxHealth, 0.f, 1.f)),
			modelStack.Scale(glm::vec3(playerHealth / playerMaxHealth, 1.f, 1.f)),
		});
			forwardSP.Set1i("noNormals", 1);
			forwardSP.Set1i("useCustomColour", 1);
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.f, 1.f, 0.f), 1.f));
			forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
			forwardSP.Set1i("customDiffuseTexIndex", -1);
			Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
			forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
			forwardSP.Set1i("useCustomColour", 0);
			forwardSP.Set1i("noNormals", 0);
		modelStack.PopModel();
		modelStack.PopModel();

		///Render items in inv
		for(short i = 0; i < 5; ++i){
			forwardSP.Set1i("noNormals", 1);
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(float(i) * 100.f - 300.f, -float(winHeight) / 2.3f, -10.f)),
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
				case ItemType::HealthPack:
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

	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	glCullFace(GL_FRONT);
	forwardSP.Set1i("sky", 1);
	forwardSP.Set1i("skybox", 1);
	forwardSP.UseTex(cubemapRefID, "cubemapSampler", GL_TEXTURE_CUBE_MAP);
	Meshes::meshes[(int)MeshType::Cube]->SetModel(modelStack.GetTopModel());
	Meshes::meshes[(int)MeshType::Cube]->Render(forwardSP);
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

	models[(int)ModelType::Tree]->InstancedRender(forwardSP);

	entityManager->Render(forwardSP, cam);

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

	cam.SetPos(glm::vec3(0.f, 0.f, 5.f));
	cam.SetTarget(glm::vec3(0.f));
	cam.SetUp(glm::vec3(0.f, 1.f, 0.f));
	view = cam.LookAt();
	projection = glm::ortho(-float(winWidth) / 2.f, float(winWidth) / 2.f, -float(winHeight) / 2.f, float(winHeight) / 2.f, .1f, 9999.f);
	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

	if(RMB && inv[currSlot] == ItemType::Sniper){
		forwardSP.Set1i("nightVision", 1);

		forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
		forwardSP.Set1i("noNormals", 1);

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(0.f, 0.f, -9.f)),
			modelStack.Scale(glm::vec3(float(winHeight) * 0.7f, float(winHeight) * 0.7f, 1.f)),
		});
			forwardSP.Set1i("customDiffuseTexIndex", 1);
			Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(0.f, 0.f, -9.1f)),
			modelStack.Scale(glm::vec3(float(winWidth) / 2.f, float(winHeight) / 2.f, 1.f)),
		});
			forwardSP.Set1i("customDiffuseTexIndex", -1);
			forwardSP.Set1i("useCustomColour", 1);
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.f), 1.f));
			Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
			forwardSP.Set1i("useCustomColour", 0);
		modelStack.PopModel();

		forwardSP.Set1i("noNormals", 0);
		forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
	} else{
		forwardSP.Set1i("nightVision", 0);

		///Render red hearts
		for(int i = 0; i < playerLives; ++i){
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(-float(winWidth) / 2.2f + 75.f * float(i), float(winHeight) / 2.2f, -9.f)),
				modelStack.Scale(glm::vec3(25.f, 25.f, 1.f)),
			});
				forwardSP.Set1i("noNormals", 1);
				forwardSP.Set1i("useCustomColour", 1);
				forwardSP.Set4fv("customColour", glm::vec4(1.f, 0.f, 0.f, 1.f));
				forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
				forwardSP.Set1i("customDiffuseTexIndex", 2);
					Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
				forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
				forwardSP.Set1i("useCustomColour", 0);
				forwardSP.Set1i("noNormals", 0);
			modelStack.PopModel();
		}

		///Render grey hearts
		for(int i = 0; i < playerMaxLives; ++i){
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(-float(winWidth) / 2.2f + 75.f * float(i), float(winHeight) / 2.2f, -10.f)),
				modelStack.Scale(glm::vec3(25.f, 25.f, 1.f)),
			});
				forwardSP.Set1i("noNormals", 1);
				forwardSP.Set1i("useCustomColour", 1);
				forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(.3f), 1.f));
				forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
				forwardSP.Set1i("customDiffuseTexIndex", 2);
					Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
				forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
				forwardSP.Set1i("useCustomColour", 0);
				forwardSP.Set1i("noNormals", 0);
			modelStack.PopModel();
		}

		///Render inv slots
		for(short i = 0; i < 5; ++i){
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(float(i) * 100.f - 300.f, -float(winHeight) / 2.3f, -10.f)),
				modelStack.Scale(glm::vec3(50.f, 50.f, 1.f)),
			});
				forwardSP.Set1i("noNormals", 1);
				if(i == currSlot){
					forwardSP.Set1i("useCustomColour", 1);
					forwardSP.Set4fv("customColour", glm::vec4(1.f, 1.f, 0.f, 1.f));
				}
				forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
				forwardSP.Set1i("customDiffuseTexIndex", 3);
					Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
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
					Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
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
						Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::Quad]->Render(forwardSP);
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

	#ifdef DEBUGGING
	textChief.RenderText(textSP, {
		"CamTarget: " + std::to_string(cam.GetTarget().x) + " " + std::to_string(cam.GetTarget().y) + " " + std::to_string(cam.GetTarget().z),
		25.f,
		25.f,
		.5f,
		glm::vec4(1.f, 1.f, 0.f, 1.f),
		0,
	});
	textChief.RenderText(textSP, {
		"CamFront: " + std::to_string(cam.CalcFront().x) + " " + std::to_string(cam.CalcFront().y) + " " + std::to_string(cam.CalcFront().z),
		25.f,
		75.f,
		.5f,
		glm::vec4(1.f, 1.f, 0.f, 1.f),
		0,
	});
	#else
	///Render bullet info
	if(currGun){
		textChief.RenderText(textSP, {
			std::to_string(currGun->GetLoadedBullets()) + " / " + std::to_string(currGun->GetUnloadedBullets()),
			float(winWidth) / 2.4f,
			125.f,
			1.f,
			glm::vec4(1.f, 1.f, 0.f, 1.f),
			0,
		});
	}
	textChief.RenderText(textSP, {
		"Time Left: " + std::to_string(timeLeft).substr(0, 5),
		25.f,
		25.f,
		1.f,
		glm::vec4(1.f, 1.f, 0.f, 1.f),
		0,
	});
	textChief.RenderText(textSP, {
		"Score: " + std::to_string(score),
		25.f,
		75.f,
		1.f,
		glm::vec4(1.f, 1.f, 0.f, 1.f),
		0,
	});
	#endif

	const float FPS = 1.0f / dt;
	textChief.RenderText(textSP, {
		"FPS: " + std::to_string(FPS).substr(0, std::to_string((int)FPS).length() + 3),
		25.f,
		125.f,
		1.f,
		glm::vec4(1.f, 1.f, 0.f, 1.f),
		0,
	});
}

void Scene::GameOverRender(){
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
		"Play Again",
		25.f,
		125.f,
		textScaleFactors[0],
		textColours[0],
		0,
	});
	textChief.RenderText(textSP, {
		"Scores",
		25.f,
		75.f,
		textScaleFactors[1],
		textColours[1],
		0,
	});
	textChief.RenderText(textSP, {
		"Quit",
		25.f,
		25.f,
		textScaleFactors[2],
		textColours[2],
		0,
	});

	textChief.RenderText(textSP, {
		"Game Over",
		30.f,
		float(winHeight) / 1.2f,
		2.f,
		glm::vec4(1.f, .5f, 0.f, 1.f),
		0,
	});
	textChief.RenderText(textSP, {
		"Final Score: " + std::to_string(score),
		30.f,
		float(winHeight) / 1.2f - 100.f,
		2.f,
		glm::vec4(1.f, .5f, 0.f, 1.f),
		0,
	});

	if(scores.size() == 1 || (score == scores.front() && score != scores[1])){
		textChief.RenderText(textSP, {
			"New High Score!",
			30.f,
			float(winHeight) / 1.2f - 200.f,
			2.f,
			glm::vec4(1.f, .5f, 0.f, 1.f),
			0,
		});
	}
	glDepthFunc(GL_LESS);
}

void Scene::ScoreboardRender(){
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
		"Back",
		25.f,
		25.f,
		textScaleFactors[2],
		textColours[2],
		0,
	});

	float currOffset = 0.f;
	textChief.RenderText(textSP, {
		"Scores",
		30.f,
		float(winHeight) / 1.2f,
		1.f,
		glm::vec4(1.f, .5f, 0.f, 1.f),
		0,
	});
	const size_t& mySize = scores.size();
	for(size_t i = 0; i < mySize; ++i){
		currOffset += 80.f;
		textChief.RenderText(textSP, {
			std::to_string(scores[i]),
			30.f,
			float(winHeight) / 1.2f - currOffset,
			1.f,
			glm::vec4(1.f, .5f, 0.f, 1.f),
			0,
		});
	}
	glDepthFunc(GL_LESS);
}

void Scene::MinimapRender(){
	static float Divisor = 5.f;

	if(screen == Screen::Game){
		forwardSP.Use();
		const int& pAmt = 0;
		const int& dAmt = 0;
		const int& sAmt = 0;
		forwardSP.Set1i("nightVision", 0);
		forwardSP.Set1f("shininess", 32.f); //More light scattering if lower
		forwardSP.Set3fv("globalAmbient", Light::globalAmbient);
		forwardSP.Set3fv("camPos", cam.GetPos());
		forwardSP.Set1i("pAmt", pAmt);
		forwardSP.Set1i("dAmt", dAmt);
		forwardSP.Set1i("sAmt", sAmt);

		switch(minimapView){
			case MinimapViewType::TopFollowingOrtho:
				minimapCam.SetPos(glm::vec3(cam.GetPos().x, 1000.f, cam.GetPos().z));
				minimapCam.SetTarget(glm::vec3(cam.GetPos().x, 0.f, cam.GetPos().z));
				minimapCam.SetUp(glm::vec3(0.f, 0.f, -1.f));
				view = minimapCam.LookAt();
				projection = glm::ortho(-float(winWidth) / Divisor, float(winWidth) / Divisor, -float(winHeight) / Divisor, float(winHeight) / Divisor, .1f, 99999.f);
				break;
			case MinimapViewType::TopFollowingPerspective:
				minimapCam.SetPos(glm::vec3(cam.GetPos().x, 1000.f, cam.GetPos().z));
				minimapCam.SetTarget(glm::vec3(cam.GetPos().x, 0.f, cam.GetPos().z));
				minimapCam.SetUp(glm::vec3(0.f, 0.f, -1.f));
				view = minimapCam.LookAt();
				projection = glm::perspective(angularFOV, cam.GetAspectRatio(), .1f, 99999.f);
				break;
			case MinimapViewType::TopStaticOrtho:
				minimapCam.SetPos(glm::vec3(0.f, 1000.f, 0.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::vec3(0.f, 0.f, -1.f));
				view = minimapCam.LookAt();
				projection = glm::ortho(-float(winWidth) * .9f, float(winWidth) * .9f, -float(winHeight) * .9f, float(winHeight) * .9f, .1f, 99999.f);
				break;
			case MinimapViewType::TopStaticPerspective:
				minimapCam.SetPos(glm::vec3(0.f, 2000.f, 0.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::vec3(0.f, 0.f, -1.f));
				view = minimapCam.LookAt();
				projection = glm::perspective(angularFOV, cam.GetAspectRatio(), .1f, 99999.f);
				break;
			case MinimapViewType::IsometricOrtho:
				minimapCam.SetPos(glm::vec3(1000.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::normalize(glm::vec3(-1.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::ortho(-float(winWidth) * .9f, float(winWidth) * .9f, -float(winHeight) * .9f, float(winHeight) * .9f, .1f, 99999.f);
				break;
			case MinimapViewType::IsometricPerspective:
				minimapCam.SetPos(glm::vec3(1000.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::normalize(glm::vec3(-1.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::perspective(angularFOV, cam.GetAspectRatio(), .1f, 99999.f);
				break;
			case MinimapViewType::ThirdPersonFollowingOrtho:
				minimapCam.SetPos(glm::vec3(cam.GetPos().x, 500.f, cam.GetPos().z + 500.f));
				minimapCam.SetTarget(glm::vec3(cam.GetPos().x, 0.f, cam.GetPos().z));
				minimapCam.SetUp(glm::normalize(glm::vec3(0.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::ortho(-float(winWidth) * .8f, float(winWidth) * .8f, -float(winHeight) * .8f, float(winHeight) * .8f, .1f, 99999.f);
				break;
			case MinimapViewType::ThirdPersonFollowingPerspective:
				minimapCam.SetPos(glm::vec3(cam.GetPos().x, 500.f, cam.GetPos().z + 500.f));
				minimapCam.SetTarget(glm::vec3(cam.GetPos().x, 0.f, cam.GetPos().z));
				minimapCam.SetUp(glm::normalize(glm::vec3(0.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::perspective(angularFOV, cam.GetAspectRatio(), .1f, 99999.f);
				break;
			case MinimapViewType::ThirdPersonStaticOrtho:
				minimapCam.SetPos(glm::vec3(0.f, 1000.f, 1000.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::normalize(glm::vec3(0.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::ortho(-float(winWidth) * .8f, float(winWidth) * .8f, -float(winHeight) * .8f, float(winHeight) * .8f, .1f, 99999.f);
				break;
			case MinimapViewType::ThirdPersonStaticPerspective:
				minimapCam.SetPos(glm::vec3(0.f, 1000.f, 1000.f));
				minimapCam.SetTarget(glm::vec3(0.f));
				minimapCam.SetUp(glm::normalize(glm::vec3(0.f, 1.f, -1.f)));
				view = minimapCam.LookAt();
				projection = glm::perspective(angularFOV, cam.GetAspectRatio(), .1f, 99999.f);
				break;
		}
		forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);

		entityManager->Render(forwardSP, minimapCam);

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(cam.GetPos().x, 0.f, cam.GetPos().z)),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(cam.CalcFront().x, cam.CalcFront().z)))),
			modelStack.Scale(glm::vec3(20.f)),
		});
			Meshes::meshes[(int)MeshType::Sphere]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::Sphere]->Render(forwardSP);
		modelStack.PopModel();
	}
}

void Scene::ForwardRender(){
	forwardSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();

	forwardSP.Set1f("shininess", 32.f); //More light scattering if lower
	forwardSP.Set3fv("globalAmbient", Light::globalAmbient);
	forwardSP.Set3fv("camPos", cam.GetPos());
	forwardSP.Set1i("pAmt", pAmt);
	forwardSP.Set1i("dAmt", dAmt);
	forwardSP.Set1i("sAmt", sAmt);

	int i;
	for(i = 0; i < pAmt; ++i){
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(i = 0; i < dAmt; ++i){
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(directionalLights[i]);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(i = 0; i < sAmt; ++i){
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		forwardSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		forwardSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch(screen){
		case Screen::MainMenu:
			MainMenuRender();
			break;
		case Screen::Game:
			GameRender();
			break;
		case Screen::GameOver:
			GameOverRender();
			break;
		case Screen::Scoreboard: {
			ScoreboardRender();
			break;
		}
	}

	glBlendFunc(GL_ONE, GL_ZERO);
}

void Scene::BlurRender(const uint& brightTexRefID, const bool& horizontal){
	blurSP.Use();
	blurSP.Set1i("horizontal", horizontal);
	blurSP.UseTex(brightTexRefID, "texSampler");
	Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
	Meshes::meshes[(int)MeshType::Quad]->Render(blurSP, false);
	blurSP.ResetTexUnits();
}

void Scene::DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID, const glm::vec3& translate, const glm::vec3& scale){
	if(!glm::length(translate) || screen == Screen::Game){
		screenSP.Use();
		screenSP.Set1f("exposure", .5f);
		screenSP.UseTex(screenTexRefID, "screenTexSampler");
		screenSP.UseTex(blurTexRefID, "blurTexSampler");
		if(screen == Screen::Game){
			screenSP.Set1i("nightVision", int(RMB && inv[currSlot] == ItemType::Sniper));
		} else{
			screenSP.Set1i("nightVision", 0);
		}
		modelStack.PushModel({
			modelStack.Translate(translate),
			modelStack.Scale(scale),
		});
			Meshes::meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::Quad]->Render(screenSP, false);
		modelStack.PopModel();
		screenSP.ResetTexUnits();
	}
}

void Scene::RenderEntities(){
}