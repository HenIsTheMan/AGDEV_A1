#pragma once
#include <Engine.h>

#include "Cam.h"

#include "../Entity/Entities/Shotgun.h"
#include "../Entity/Entities/Scar.h"
#include "../Entity/Entities/Sniper.h"

#include "../Entity/EntityManager.h"

#include "../Shared/Meshes.h"

#include "ModelStack.h"

#include "../LOD/LOD.h"

#include "../Shared/RotateVecIn2D.hpp"

#define BIT(x) 1 << x

class Scene final{
public:
	Scene();
	~Scene();
	bool Init();
	void Update(GLFWwindow* const& win);
	void ForwardRender();
private:
	Cam cam;
	ISoundEngine* soundEngine;
	std::vector<ISound*> coinMusic;
	std::vector<ISoundEffectControl*> coinSoundFX;
	std::vector<ISound*> fireMusic;
	std::vector<ISoundEffectControl*> fireSoundFX;
	TextChief textChief;

	int polyModes[2];

	LOD treeLOD;

	enum struct ModelType{
		Shotgun = 0,
		Scar,
		Sniper,
		Flower,
		Grass,
		Rock,
		Tree_Low,
		Tree_Medium,
		Tree_High,
		Dragon_Low,
		Dragon_Medium,
		Dragon_High,
		Amt
	};
	Model* models[(int)ModelType::Amt];

	ShaderProg forwardSP;
	ShaderProg viewingFrustumSP;
	ShaderProg textSP;

	uint cubemapRefID;

	enum struct ItemType: int{
		None = -1,
		Shotgun,
		Scar,
		Sniper,
		ShotgunAmmo,
		ScarAmmo,
		SniperAmmo,
		Amt
	};
	int currSlot;
	ItemType inv[5];

	enum struct Screen: int{
		MainMenu,
		Game,
		Amt
	};

	Screen screen;
	float textScaleFactors[2];
	glm::vec4 textColours[2];
	Gun* currGun;
	Gun* guns[3];
	glm::vec4 reticleColour;

	void MainMenuUpdate(GLFWwindow* const& win);
	void GameUpdate(GLFWwindow* const& win);

	void MainMenuRender();
	void GameRender();

	void InitEntities();

	void CreateEntities();
	void CreateTreesAndCubes();
	void CreateDecorations();

	glm::mat4 view;
	glm::mat4 projection;

	EntityManager* entityManager;

	NodeManager* nodeManager;
	RegionManager* regionManager;

	bool shldUpdateEntityManager;
	bool shldRenderEntityManager;

	bool isCamDetached;
	bool shldRenderViewingFrustum;
	float elapsedTime;

	glm::vec3 frustumColor;

	Light* dLightFromTop;
	Light* dLightFromBottom;

	ModelStack modelStack;

	const Entity* myPlayer;
};