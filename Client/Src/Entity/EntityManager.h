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

	void DeactivateEntity(Entity* const& entity, const bool movable);
private:
	EntityManager();

	Entity* const ActivateEntity(const bool movable);

	void ActivateEntityProcedure(Entity* const entity);
	void DeactivateEntityProcedure(Entity* const entity);

	bool shldRenderColliders;
	float elapsedTime;

	std::vector<Entity*> entityPool;
	Node* rootNode;
	RegionManager* regionManager;
	EntityFactory* entityFactory;

	ColliderManager* colliderManager;
	ModelStack modelStack;
};