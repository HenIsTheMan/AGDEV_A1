#pragma once

#include <Engine.h>

#include "Entity.h"

#include "../Collider/ColliderManager.h"
#include "../Quadtree/NodeManager.h"
#include "../Quadtree/RegionManager.h"

#include "../Collider/Colliders/BoxCollider.h"
#include "../Collider/Colliders/SphereCollider.h"

#include "../ObjPool/ObjPool.h"

struct EntityCreationAttribs final{
	glm::vec3 pos;
	glm::vec3 scale;
	glm::vec4 colour;
	int diffuseTexIndex;
};

class EntityFactory final: public Singleton<EntityFactory>{
	friend Singleton<EntityFactory>;
public:
	~EntityFactory();

	const Entity* CreatePlayer(const EntityCreationAttribs& attribs);

	void CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront);

	void CreateCoin(const EntityCreationAttribs& attribs);
	void CreateFire(const EntityCreationAttribs& attribs);

	void CreateEnemy(const EntityCreationAttribs& attribs);

	void CreateThinObj(const EntityCreationAttribs& attribs);

	void CreateTree(const EntityCreationAttribs& attribs);
private:
	ColliderManager* colliderManager;
	NodeManager* nodeManager;
	RegionManager* regionManager;

	ObjPool<Entity>* entityPool;

	EntityFactory();

	Entity* ActivateEntity(const bool movable);
	void ActivateEntityProcedure(Entity* const entity);
};