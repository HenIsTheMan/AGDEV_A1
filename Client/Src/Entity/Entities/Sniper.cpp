#include "Sniper.h"

Sniper::Sniper(){
	reloadTime = 3.f;
	shotCooldownTime = 0.f;
	loadedBullets = 1;
	maxLoadedBullets = 1;
	unloadedBullets = 10;
	maxUnloadedBullets = 30;
}

void Sniper::Shoot(const float& elapsedTime, Entity* const& entity, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Sniper.wav", false);

		entity->type = Entity::EntityType::Bullet;
		entity->active = true;
		entity->life = 5.f;
		entity->maxLife = 5.f;
		entity->colour = glm::vec4(0.f, 0.f, 1.f, .3f);
		entity->diffuseTexIndex = -1;
		entity->collisionNormal = glm::vec3(1.f, 0.f, 0.f);
		entity->scale = glm::vec3(.2f);
		entity->light = nullptr;

		entity->pos = camPos + 10.f * camFront;
		entity->vel = 500.f * camFront;
		entity->mass = 5.f;
		entity->force = glm::vec3(0.f);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}