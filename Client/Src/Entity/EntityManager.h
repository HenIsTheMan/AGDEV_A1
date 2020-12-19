#pragma once

#include "../Entity/Entity.h"
#include "../Quadtree/RegionControl.h"
#include "../Collider/ColliderManager.h"

class EntityManager final: public Singleton<EntityManager>{
	friend Singleton<EntityManager>;
public:
	~EntityManager();

	struct EntityCreationAttribs final{
		glm::vec3 pos;
		glm::vec3 scale;
		glm::vec4 colour;
		int diffuseTexIndex;
	};

	void Init();
	void Update();
	void Render(ShaderProg& SP, const Cam& cam);

	const Entity* CreatePlayer(const EntityCreationAttribs& attribs);

	void CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront);

	void CreateCoin(const EntityCreationAttribs& attribs);
	void CreateFire(const EntityCreationAttribs& attribs);

	void CreateEnemy(const EntityCreationAttribs& attribs);

	void CreateThinObj(const EntityCreationAttribs& attribs);

	void CreateTree(const EntityCreationAttribs& attribs);

	void DeactivateEntity(Entity* const& entity, const bool movable);

	void SetUpRegionsForStationary();
private:
	EntityManager();

	Entity* const ActivateEntity(const bool movable);

	void ActivateEntityProcedure(Entity* const entity);
	void DeactivateEntityProcedure(Entity* const entity);

	bool shldRenderColliders;

	std::vector<Entity*> entityPool;
	Node* rootNode;
	RegionControl* regionControl;

	ColliderManager* colliderManager;
	ModelStack modelStack;
};