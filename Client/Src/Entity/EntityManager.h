#pragma once

#include "../Entity/Entity.h"
#include "../Quadtree/RegionManager.h"
#include "../Collider/ColliderManager.h"

#include "EntityFactory.h"

class EntityManager final: public Singleton<EntityManager>{
	friend Singleton<EntityManager>;
public:
	~EntityManager();

	void Init();
	void Update();
	void Render(ShaderProg& SP, const Cam& cam);
private:
	EntityManager();

	void DeactivateEntity(Entity* const& entity, const bool movable);
	void DeactivateEntityProcedure(Entity* const entity);

	bool shldRenderColliders;
	float elapsedTime;

	Node* rootNode;

	EntityFactory* entityFactory;

	RegionManager* regionManager;
	ColliderManager* colliderManager;

	ObjPool<Entity>* entityPool;

	ModelStack modelStack;
};