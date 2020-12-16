#include "Shotgun.h"

Shotgun::Shotgun(){
	reloadTime = 2.f;
	shotCooldownTime = .7f;
	loadedBullets = 12;
	maxLoadedBullets = 12;
	unloadedBullets = 18;
	maxUnloadedBullets = 360;
}

void Shotgun::Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine){
	static float bulletBT = 0.f;
	if(canShoot && loadedBullets && bulletBT <= elapsedTime){
		soundEngine->play2D("Audio/Sounds/Shotgun.wav", false);

		Entity* const entity = entityManager->FetchEntity();
		entity->type = Entity::EntityType::Bullet;
		entity->life = 5.f;
		entity->maxLife = 5.f;
		entity->colour = glm::vec4(glm::vec3(.4f), .3f);
		entity->diffuseTexIndex = -1;
		entity->collisionNormal = glm::vec3(1.f, 0.f, 0.f);
		entity->scale = glm::vec3(.2f);
		entity->light = nullptr;

		entity->pos = camPos;
		entity->vel = 200.f * glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-1.f, 1.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //With bullet bloom
		entity->mass = 5.f;
		entity->force = glm::vec3(0.f);

		--loadedBullets;
		bulletBT = elapsedTime + shotCooldownTime;
	}
}