#include "Shotgun.h"

Shotgun::Shotgun(){
	reloadTime = 2.f;
	shotCooldownTime = .7f;
	loadedBullets = 24;
	maxLoadedBullets = 24;
	unloadedBullets = 4000;
	maxUnloadedBullets = 4000;
}

void Shotgun::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Shotgun.wav", false);

		entityFactory->CreateShotgunBullet(camPos, camFront);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}