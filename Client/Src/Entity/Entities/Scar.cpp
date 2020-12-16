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

		Entity* const entity = entityManager->FetchEntity();
		entity->type = Entity::EntityType::Bullet;
		entity->active = true;
		entity->life = 5.f;
		entity->maxLife = 5.f;
		entity->colour = glm::vec4(glm::vec3(1.f), .3f);
		entity->diffuseTexIndex = -1;
		entity->collisionNormal = glm::vec3(1.f, 0.f, 0.f);
		entity->scale = glm::vec3(.2f);
		entity->light = nullptr;

		entity->pos = camPos + 10.f * camFront;
		entity->vel = 180.f * glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-2.f, 2.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //With bullet bloom
		entity->mass = 5.f;
		entity->force = glm::vec3(0.f);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}