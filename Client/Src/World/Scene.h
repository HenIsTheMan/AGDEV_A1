#pragma once
#include <Engine.h>

#include "Cam.h"

#include "../Entity/Entities/Shotgun.h"
#include "../Entity/Entities/Scar.h"
#include "../Entity/Entities/Sniper.h"

#include "../Entity/EntityManager.h"

#include "../Shared/Meshes.h"

#include "ModelStack.h"

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

	enum struct ModelType{
		Shotgun = 0,
		Scar,
		Sniper,
		Flower,
		Grass,
		Rock,
		Tree,
		Dragon_Low,
		Dragon_Medium,
		Dragon_High,
		Amt
	};
	Model* models[(int)ModelType::Amt];

	ShaderProg forwardSP;
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

	void MainMenuUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT);
	void GameUpdate(GLFWwindow* const& win);

	void MainMenuRender();
	void GameRender();

	void InitEntities();

	glm::mat4 view;
	glm::mat4 projection;

	EntityManager* entityManager;

	bool isCamDetached;
	float elapsedTime;

	Light* dLight;

	ModelStack modelStack;
};

enum struct Axis{
	x = 0,
	y,
	z,
	Amt
};

inline static glm::vec3 RotateVecIn2D(const glm::vec3& vec, const float& angleInRad, const Axis& axis){
	switch(axis){
		case Axis::x:
			return glm::vec3(vec.x, vec.y * cos(angleInRad) + vec.z * -sin(angleInRad), vec.y * sin(angleInRad) + vec.z * cos(angleInRad));
		case Axis::y:
			return glm::vec3(vec.x * cos(angleInRad) + vec.z * sin(angleInRad), vec.y, vec.x * -sin(angleInRad) + vec.z * cos(angleInRad));
		case Axis::z:
			return glm::vec3(vec.x * cos(angleInRad) + vec.y * -sin(angleInRad), vec.x * sin(angleInRad) + vec.y * cos(angleInRad), vec.z);
		default:
			(void)puts("Invalid axis input for vec rotation!");
			return glm::vec3();
	}
}