#pragma once
#include <Core.h>
#include <Engine.h>

#include "../Collision/Collider/Collider.h"

class Entity final{
	template <class T>
	friend class ObjPool;

	friend class Collision;
	friend class EntityFactory;
	friend class EntityManager;
	friend class Node;
	friend class Region;
	friend class RegionManager;
	friend class Shotgun;
	friend class Scar;
	friend class Sniper;
	friend class Gun;
	friend void UpdatePlayerHoriz(Entity* const player, const bool isCamDetached);
	friend void UpdatePlayerVert(Entity* const player);
public:
	void Reset();

	///Getters
	const glm::vec3& GetPos() const;
	const glm::vec3& GetScale() const;
	const glm::vec3& GetFacingDir() const;

	///Setter
	void SetFacingDir(const glm::vec3& facingDir);
	void SetPos(const glm::vec3& pos);
	void SetScale(const glm::vec3& scale);
private:
	enum struct EntityType: int{
		Bullet,
		Coin,
		Fire,
		Player,
		ThinObj,
		EnemyBody,
		EnemyPart,
		Tree,
		Amt
	};

	Entity();

	EntityType type;
	bool movable;
	float life;
	float maxLife;
	glm::vec4 colour;
	int diffuseTexIndex;
	glm::vec3 scale;

	glm::vec3 pos;
	glm::vec3 vel;
	float mass;
	glm::vec3 force;

	Collider* collider;
	glm::vec3 facingDir;
	float moveSpd;

	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;

	glm::vec3 prevPos;
};