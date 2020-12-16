#pragma once

#include "../Entity/Entity.h"
#include "../Quadtree/RegionControl.h"

class EntityManager final: public Singleton<EntityManager>{
	friend Singleton<EntityManager>;
public:
	~EntityManager();

	struct EntityCreationAttribs final{
		glm::vec3 pos;
		glm::vec3 collisionNormal;
		glm::vec3 scale;
		glm::vec4 colour;
		int diffuseTexIndex;
	};

	void Init();
	void Update();
	void Render(ShaderProg& SP) const;

	void CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront);
	void CreateAmmo(const Entity::EntityType type, const EntityCreationAttribs& attribs);
	void CreateCoin(const EntityCreationAttribs& attribs);
	void CreateFire(const EntityCreationAttribs& attribs);
	void CreateEnemy(const EntityCreationAttribs& attribs);

	void DeactivateEntity(Entity* const& entity, const bool movable);
private:
	EntityManager();

	Entity* const ActivateEntity(const bool movable);

	void ActivateEntityProcedure(Entity* const entity);
	void DeactivateEntityProcedure(Entity* const entity);

	std::vector<Entity*> entityPool;

	RegionControl* regionControl;
};