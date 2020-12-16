#include "Sniper.h"

Sniper::Sniper(){
	reloadTime = 3.f;
	shotCooldownTime = 0.f;
	loadedBullets = 1;
	maxLoadedBullets = 1;
	unloadedBullets = 10;
	maxUnloadedBullets = 30;
}

void Sniper::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Sniper.wav", false);

		entityManager->CreateSniperBullet(camPos, camFront);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}