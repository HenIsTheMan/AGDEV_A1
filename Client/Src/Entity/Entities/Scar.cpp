#include "Scar.h"

Scar::Scar(){
	reloadTime = 1.f;
	shotCooldownTime = .1f;
	loadedBullets = 40;
	maxLoadedBullets = 40;
	unloadedBullets = 120;
	maxUnloadedBullets = 2400;
}

void Scar::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Sniper.wav", false);

		entityManager->CreateScarBullet(camPos, camFront);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}