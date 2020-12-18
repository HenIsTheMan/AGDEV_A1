#pragma once
#include <Core.h>
#include <Engine.h>

#include "../Collider/Collider.h"

class Entity final{
	friend class EntityManager;
	friend class Region;
	friend class RegionControl;
	friend class Shotgun;
	friend class Scar;
	friend class Sniper;
	friend class Gun;
private:
	enum class EntityType{
		Bullet = 0,
		Coin,
		Fire,
		Enemy,
		ShotgunAmmo,
		ScarAmmo,
		SniperAmmo,
		Player,
		Tree,
		Amt
	};

	Entity();

	EntityType type;
	bool active;
	bool movable;
	float life;
	float maxLife;
	glm::vec4 colour;
	int diffuseTexIndex;
	glm::vec3 scale;

	glm::vec3 pos;
	glm::vec3 dir;
	float spd;
	float mass;
	glm::vec3 force;

	Collider* collider;
};