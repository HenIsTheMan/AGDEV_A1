#pragma once

#include "../Entity/Entity.h"

#include "../Collision/Collider/ColliderManager.h"
#include "../Quadtree/NodeManager.h"
#include "../Quadtree/RegionManager.h"

#include "EntityFactory.h"

class EntityManager final: public Singleton<EntityManager>{
	friend Singleton<EntityManager>;
public:
	~EntityManager();

	void Init();
	void Update(const Cam& cam);
	void Render(ShaderProg& SP, const Cam& cam);

	EntityFactory* RetrieveEntityFactory();

	void SetUpRegionsForStationary();

	bool isCamDetached;
private:
	EntityManager();

	void DeactivateEntity(Entity* const& entity);
	void DeactivateEntityProcedure(Entity* const entity);

	bool shldRenderColliders;
	float elapsedTime;

	EntityFactory* entityFactory;

	ColliderManager* colliderManager;
	NodeManager* nodeManager;
	RegionManager* regionManager;

	ObjPool<Entity>* entityPool;

	ModelStack modelStack;
};