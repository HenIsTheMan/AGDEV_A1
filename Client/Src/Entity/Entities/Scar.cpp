#include "Scar.h"

Scar::Scar(){
	reloadTime = 1.f;
	shotCooldownTime = .1f;
	loadedBullets = 400;
	maxLoadedBullets = 400;
	unloadedBullets = 400000;
	maxUnloadedBullets = 400000;
}

void Scar::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Sniper.wav", false);

		entityFactory->CreateScarBullet(camPos, camFront);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}