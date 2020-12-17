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
	void BlurRender(const uint& brightTexRefID, const bool& horizontal);
	void DefaultRender(const uint& screenTexRefID, const uint& blurTexRefID, const glm::vec3& translate, const glm::vec3& scale);
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
		Amt
	};
	Model* models[(int)ModelType::Amt];

	ShaderProg blurSP;
	ShaderProg forwardSP;
	ShaderProg normalsSP;
	ShaderProg screenSP;
	ShaderProg textSP;

	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;
	uint cubemapRefID;

	enum struct ItemType{
		None = -1,
		Shotgun,
		Scar,
		Sniper,
		ShotgunAmmo,
		ScarAmmo,
		SniperAmmo,
		HealthPack,
	};
	int currSlot;
	ItemType inv[5];
	enum struct PlayerState{
		NoMovement = BIT(1),
		Walking = BIT(2),
		Sprinting = BIT(3),

		Standing = BIT(4),
		Jumping = BIT(5),
		Falling = BIT(6),
		Crouching = BIT(7),
		Proning = BIT(8),
	};
	int playerStates;
	enum struct Screen{
		MainMenu = 0,
		Game,
		GameOver,
		Scoreboard,
		Amt
	};

	Screen screen;
	float timeLeft;
	int score;
	std::vector<int> scores;
	float textScaleFactors[3];
	glm::vec4 textColours[3];
	bool sprintOn;
	float playerHealth;
	float playerMaxHealth;
	int playerLives;
	int playerMaxLives;
	Gun* currGun;
	Gun* guns[3];
	glm::vec4 reticleColour;

	void MainMenuAndGameOverUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT);
	void GameUpdate(GLFWwindow* const& win);
	void ScoreboardUpdate(GLFWwindow* const& win, const POINT& mousePos, float& buttonBT);

	void MainMenuRender();
	void GameRender();
	void GameOverRender();
	void ScoreboardRender();

	void InitEntities();
	void RenderEntities();

	glm::mat4 view;
	glm::mat4 projection;

	EntityManager* entityManager;

	bool isCamDetached;
	float elapsedTime;

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