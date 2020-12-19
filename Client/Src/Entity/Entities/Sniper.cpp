#include "Sniper.h"

Sniper::Sniper(){
	reloadTime = 3.f;
	shotCooldownTime = 0.f;
	loadedBullets = 1;
	maxLoadedBullets = 1;
	unloadedBullets = 400;
	maxUnloadedBullets = 400;
}

void Sniper::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Sniper.wav", false);

		entityFactory->CreateSniperBullet(camPos, camFront);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}